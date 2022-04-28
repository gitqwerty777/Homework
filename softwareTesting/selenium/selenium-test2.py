import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

"""
https://github.com/noffle/art-of-readme
feature
  - search in code
  - copy clone link
  - signin & signout
  - random choose a file and check with predownloaded file
    - choose from a file list
  - because there's a lot of files in a repository, random choose is needed
  - switch branch
"""

class GithubSearch(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
        #self.driver.manage().timeouts().pageLoadTimeout(10, TimeUnit.SECONDS)

    def test_search(self):
        driver = self.driver
        driver.get("https://github.com/runningskull/gnugo")
        driver.save_screenshot('mainpage.png')
        elem = driver.find_element_by_name("q")
        elem.send_keys("SAFE_ON_BOARD(i, j)") # search a function defined in a file
        elem.send_keys(Keys.RETURN)
        element = driver.find_element_by_id("code_search_results") # will block
        print(driver.current_url)
        self.assertIn("patterns/joseki.c", driver.page_source)

    """def test_get_url(self):
        pass
        #elem = driver.find_element_by_name("q")
        #Clone or download this repository"
    """

    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()
