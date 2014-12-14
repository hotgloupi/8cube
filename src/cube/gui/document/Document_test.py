
from ..window import Window
from . import Document

from cube.test import Case

class _(Case):

    def test_init(self):
        w = Window(12, 12)
        self.assertTrue(len(w.documents) == 0)
        doc = w.create_document()
        self.assertTrue(len(w.documents) == 1)
        self.assertEqual(doc, w.documents[0])

    def test_title(self):
        w = Window(12, 12)
        doc = w.create_document()
        self.assertEqual(doc.title, "")
        doc.title = "LOLOLOL"
        self.assertEqual(doc.title, "LOLOLOL")


    def test_by_id(self):
        w = Window(640, 480)
        doc = w.load_document(
            """
            <rml>
                <head>
                    <title>Test - by_id</title>
                    <style>body{font-family: FreeMono; font-size: 30pt;}</style>
                </head>
                <body>
                    <p id="paragraph1">LOL</p>
                </body>
            </rml>
            """
        )

        self.assertEqual(doc.title, "Test - by_id")
        self.assertIsNone(doc.by_id("NON-EXISTENT"))
        p = doc.by_id("paragraph1")
        self.assertIsNotNone(p)
        self.assertEqual(p.text, "LOL")
        p.text = "coucou"
        self.assertEqual(p.text, "coucou")



