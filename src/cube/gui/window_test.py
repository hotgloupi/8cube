from pathlib import Path

from .window import Window

from cube.test import Case
from cube import gl
from cube.gl.renderer.Painter_test import PainterSetup, painter_test

class _(Case):

    def setUp(self):
        font_path = (
            Path(__file__).resolve() / '../../gl/font/font_test.ttf'
        ).resolve()
        self.window = Window(640, 480)
        self.window.add_font(font_path)
        self.renderer = self.window.renderer
        self.target = self.renderer.context.new_render_target()

    @painter_test(gl.mode_2d)
    def test_load_document_from_string(self, painter):
        self.window.load_document(
            """
            <rml>
            <head><style>
            body{
                font-family: FreeMono;
                font-size: 30pt;
                color: white;
                background-color: #111;
            }
            .blue {color:blue;}
            .green {color:green;}
            .red {color:red;}
            .half { color: #ffffff88; }
            </style></head>
            <body>
            <p>Some text in white</p>
            <br />
            <p class="half">Some text in half blended white</p>
            <br />
            <p class="red">Some text in red</h1>
            <br />
            <p class="green">Some text in green</h1>
            <br />
            <p class="blue">Some text in blue</p>
            </body>
            </rml>
            """
        )
        self.window.render(painter)

    #@painter_test(gl.mode_2d)
    #def test_load_document_from_file(self, painter):
    #    self.window.renderer.clear(0xfffffff)
    #    self.window.load_document(
    #        Path(__file__).resolve().parent / 'simple_window_test.rml'
    #    )
    #    self.window.render(painter)
