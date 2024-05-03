import torch
from copy import deepcopy

def fisher_matrix(model, dataset, samples):
    """
    Compute the Fisher matrix diagonal approximation.
    """
    model.eval()
    fisher_diag = [torch.zeros_like(param) for param in model.parameters()]
    for _ in range(samples):
        # Sample data
        data, target = dataset[torch.randint(len(dataset), size=(1,))]
        # Forward pass and loss computation
        output = model(data)
        loss = torch.nn.functional.nll_loss(output, target)
        # Backward pass to compute gradients
        loss.backward()
        # Accumulate squared gradients
        for i, param in enumerate(model.parameters()):
            fisher_diag[i] += param.grad.data ** 2
    # Average accumulated squared gradients
    fisher_diag = [diag / samples for diag in fisher_diag]
    return fisher_diag

def ewc_loss(lam, model, dataset, samples):
    """
    Generate EWC loss function.
    """
    optimal_weights = deepcopy(list(model.parameters()))
    fisher_diag = fisher_matrix(model, dataset, samples)

    def loss_fn(model):
        loss = 0
        for i, (param, opt_param, fish_diag) in enumerate(zip(model.parameters(), optimal_weights, fisher_diag)):
            loss += (fish_diag * (param - opt_param) ** 2).sum()
        return (lam / 2) * loss

    return loss_fn

def fim_mask(model, dataset, samples, threshold):
    """
    Generate a mask based on Fisher information.
    """
    fisher_diag = fisher_matrix(model, dataset, samples)
    mask = [diag < threshold for diag in fisher_diag]
    return mask

def combine_masks(mask1, mask2):
    """
    Combine two masks.
    """
    if mask1 is None:
        return mask2
    elif mask2 is None:
        return mask1
    else:
        return [m1 and m2 for m1, m2 in zip(mask1, mask2)]

def apply_mask(gradients, mask):
    """
    Apply mask to gradients.
    """
    if mask is not None:
        gradients = [grad * m for grad, m in zip(gradients, mask)]
    return gradients

def clip_gradients(gradients, threshold):
    """
    Clip gradients using IncDet method.
    """
    result = []
    for grad in gradients:
        scale = threshold / torch.maximum(threshold, torch.abs(grad))
        result.append(scale * grad)
    return result