
import cubeapp
import cube

DOCUMENT = """
<rml>
    <head>
    <style>
    body {
        font-family: FreeMono;
        font-size: 30pt;
        color: white;
    }
    </style>
    </head>
    <body>
        <p>8cube framework</p>
    </body>
</rml>
"""

class Game(cubeapp.game.Game):

    def __init__(self, **kw):
        super().__init__(**kw)
        self.window.add_font(self.directory / 'FreeMono.ttf')
        self.window.load_document(DOCUMENT)

