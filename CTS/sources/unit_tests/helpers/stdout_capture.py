from cStringIO import StringIO
import sys

class StdoutCapture(list):
    def __enter__(self):
        self._original_std_out = sys.stdout
        sys.stdout = self._capture = StringIO()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.extend(self._capture.getvalue().splitlines())
        sys.stdout = self._original_std_out

    @property
    def raw(self):
        return self._capture.getvalue()
