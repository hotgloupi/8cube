from .Scene import Scene, from_string, from_file

from .Node import Node
from .Transform import Transform
from .ContentNode import ContentNodeBindable
from .ContentNode import ContentNodeDrawable
from .ContentNode import ContentNodeMaterial
from .ContentNode import ContentNodeMesh
from .ContentNode import ContentNodeLight
from .Graph import Graph
from .visit import *

from cube import gl

def content_node_type(value):
    node_type = {
        gl.Light: ContentNodeLight,
        gl.Material: ContentNodeMaterial,
        gl.Mesh: ContentNodeMesh,
    }.get(type(value))

    if node_type is not None:
        return node_type

    if isinstance(value, gl.Bindable):
        return ContentNodeBindable
    elif isinstance(value, gl.Drawable):
        return ContentNodeDrawable

    raise Exception("Couldn't find any ContentNode type for '%s'" % value)
