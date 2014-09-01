
class Application:
    def run(self):
        try:
            self._run()
        except:
            self.shutdown()
            raise

    def _run(self):
        """This method should be overriden"""
        pass

    def shutdown(self):
        pass
