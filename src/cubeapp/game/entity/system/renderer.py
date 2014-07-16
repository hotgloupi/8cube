from . import System

class Renderer(System):

    def __init__(self, renderer):
        super().__init__()
        self.renderer = renderer

    def init_component(self, entity, component, content = None):
        if content is not None:
            component.content = content

