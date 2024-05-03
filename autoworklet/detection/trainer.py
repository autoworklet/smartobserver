from copy import deepcopy
from ultralytics.models.yolo.detect import DetectionTrainer
from ultralytics.nn.tasks import DetectionModel
from ultralytics.utils import LOGGER, RANK
from ultralytics.utils.loss import v8DetectionLoss
from torch.nn import functional as F
from torch.autograd import Variable
from ultralytics.data import YOLODataset

def get_fisher_diag(model, dataset, params, empirical=True):
    fisher = {}
    for n, p in deepcopy(params).items():
        p.data.zero_()
        fisher[n] = Variable(p.data)

    model.eval()
    for input, gt_label in dataset:
        model.zero_grad()
        output = model(input).view(1, -1)
        if empirical:
            label = gt_label
        else:
            label = output.max(1)[1].view(-1)
        negloglikelihood = F.nll_loss(F.log_softmax(output, dim=1), label)
        negloglikelihood.backward()

        for n, p in model.named_parameters():
            fisher[n].data += p.grad.data ** 2 / len(dataset)

    fisher = {n: p for n, p in fisher.items()}
    return fisher

class v8DetectionLossECW(v8DetectionLoss):
    def __init__(self, model, fisher_matrix_taskA, p_old):
        super().__init__(model)
        self.ecw = model.cfg.get("ecw", 0.1)
        self.fisher_matrix_taskA = fisher_matrix_taskA
        self.params_taskA = p_old
    def ecw_loss(self, fisher_matrix_taskA, params_taskA):
        loss = 0
        for n, p in self.model.named_parameters():
            _loss = fisher_matrix_taskA[n] * (p - params_taskA[n]) ** 2
            loss += _loss.sum()
        return loss
    def __call__(self, preds, batch):
        loss = super().__call__(preds, batch)
        return loss + self.ecw * self.ecw_loss(self.fisher_matrix_taskA, self.p_old)

def init_criterion(self):
    """Initialize the loss criterion for the DetectionModel."""
    return v8DetectionLossECW(self, fisher_matrix_taskA, p_old)

def get_model(self, cfg=None, weights=None, verbose=True):
    """Return a YOLO detection model."""
    # print('get_model', cfg, weights, verbose)
    model = DetectionModel(cfg, nc=self.data["nc"], verbose=verbose and RANK == -1)
    if weights:
        model.load(weights)
    return model

DetectionModel.init_criterion = init_criterion
DetectionTrainer.get_model = get_model
args = dict(model='yolov8n.pt', data='coco8.yaml', epochs=3)
trainer = DetectionTrainer(overrides=args)


# params_taskA = {n: p for n, p in model_taskA.named_parameters() if p.requires_grad}
# p_old = {}
# for n, p in deepcopy(params_taskA).items():
#     p_old[n] = Variable(p.data)
# fisher_matrix_taskA = get_fisher_diag(model_taskA, dataset_taskA, params_taskA)
# trainer.train()
