import os

def create_dir(filepath):
    wdir = os.path.dirname(filepath)
    if not os.path.exists(wdir):
        try:
            os.makedirs(wdir)
        except OSError as exc: # Guard against race condition
            if exc.errno != errno.EEXIST:
                raise Exception("something happends while creating folder")