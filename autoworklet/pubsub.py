class PubSub:
    """A simple publish-subscribe class for event handling."""

    def __init__(self):
        self.subscribers = dict()

    def subscribe(self, event_type, callback):
        if event_type not in self.subscribers:
            self.subscribers[event_type] = []
        self.subscribers[event_type].append(callback)

    def publish(self, event_type, event):
        if event_type in self.subscribers:
            for callback in self.subscribers[event_type]:
                callback(event)

    def unsubscribe(self, event_type, callback):
        if event_type in self.subscribers:
            self.subscribers[event_type].remove(callback)
            if len(self.subscribers[event_type]) == 0:
                del self.subscribers[event_type]

    def clear(self):
        self.subscribers = dict()

    def __del__(self):
        self.clear()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.clear()
