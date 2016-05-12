import logging
import os
import shutil
import subprocess
import tempfile
import time
import unittest


class CookbookUnitTests(unittest.TestCase):

    def build_charybdefs(self):
        if not os.path.exists("../charybdefs"):
            self._log.warning("Building CharybdeFS")
            os.chdir("..")
            subprocess.call(["cmake", "CMakeLists.txt"])
            subprocess.call(["make"])
            os.chdir("cookbook")

    def setUp(self):
        logging.basicConfig()
        self._log = logging.getLogger("demo")

        self._log.warning("Setting up demo")
        
        self.build_charybdefs()

        self._log.warning("Setting up mount points and target directory")

        # create the CharybdeFS target directory
        self._data_dir = os.path.expanduser("~/.CharybdeFSCookbookDir")
        os.mkdir(self._data_dir)
        # Create the CharybdeFS mount point
        self._mount_point = os.path.expanduser("~/.CharybdeFSCookbookDirData")
        os.mkdir(self._mount_point)

        self._log.warning("Starting CharybdeFS")
        # Killing previous CharybdeFS instances
        subprocess.call(["pkill", "-9", "charybdefs"])
        # Spawning charybdeFS
        subprocess.call(["../charybdefs", self._mount_point, "-omodules=subdir,subdir=%s" % self._data_dir])
        
    def victim(self):
        return os.path.join(self._mount_point, "blub")

    def run_recipe(self, name):
        subprocess.call(["./recipes", "--clear"])
        subprocess.call(["./recipes", "--%s" % name])
        time.sleep(0.5)

    def tearDown(self):
        self._log.warning("Tearing down demo and cleaning up everything")
        subprocess.call(["umount", self._mount_point])
        subprocess.call(["pkill", "-9", "charybdefs"])
        shutil.rmtree(self._mount_point)
        shutil.rmtree(self._data_dir)
        self._log.warning("\n")

    def test_disk_full(self):
        self.run_recipe("full")
        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % self.victim()])

    def test_io_error(self):
        self.run_recipe("io-error")
        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % self.victim()])

    def test_quota(self):
        self.run_recipe("quota")
        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % self.victim()])

    def test_delay(self):
        self.run_recipe("delay")
        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % self.victim(),
                         "bs=4k", "count=100"])

    def test_random(self):
        self.run_recipe("random")
        subprocess.call(["dd", "if=/dev/zero",
                          "of=%s" % self.victim(),
                          "bs=4k", "count=1000"])

    def test_specific_syscalls(self):
        self.run_recipe("specific-syscalls")
        
        self._log.warning("Doing ls")
        subprocess.call(["ls", self._mount_point])

        self._log.warning("Doing write")
        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % self.victim(),
                         "bs=4k", "count=1000"])

        self._log.warning("Doing read")
        subprocess.call(["dd", "if=%s" % self.victim(),
                         "of=/dev/null", "bs=4k", "count=1000"])

    def test_probability(self):
        self.run_recipe("probability")

        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % self.victim(),
                         "bs=4k", "count=1000"])

    def test_file_pattern(self):
        self.run_recipe("file-pattern")

        self._log.warning("Writing postfix main.cf: good")
        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % os.path.join(self._mount_point, "main.cf"),
                         "bs=4k", "count=1000"])

        self._log.warning("Writing sendmail sendmail.cf: baaaad !")
        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % os.path.join(self._mount_point, "sendmail.cf"),
                         "bs=4k", "count=1000"])

    def test_broken_drive(self):
        self.run_recipe("broken-drive")

        subprocess.call(["dd", "if=/dev/zero",
                         "of=%s" % self.victim(),
                         "bs=4k", "count=100"])

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(CookbookUnitTests)
    unittest.TextTestRunner(verbosity=2).run(suite)
