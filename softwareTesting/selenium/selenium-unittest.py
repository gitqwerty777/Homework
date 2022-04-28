import unittest
import random
import requests
import filecmp
import time

from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

"""
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
        self.driver.implicitly_wait(10) # wait at most 10 seconds until fully loaded
        #self.driver.manage().timeouts().pageLoadTimeout(10, TimeUnit.SECONDS)

    def test_search(self):
        driver = self.driver
        driver.get("https://github.com/runningskull/gnugo")
        
        elem = driver.find_element_by_name("q")
        elem.send_keys("SAFE_ON_BOARD(i, j)")
        elem.send_keys(Keys.RETURN)
        element = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "code_search_results")))
        print(driver.current_url)
        driver.save_screenshot('selenium-search.png')
        self.assertIn("patterns/joseki.c", driver.page_source)

    def test_clone_url(self):
        driver = self.driver
        driver.get("https://github.com/runningskull/gnugo")
        driver.find_element_by_css_selector('.btn.btn-sm.btn-primary.select-menu-button.js-menu-target').click()
        clone_url = driver.find_element_by_css_selector('.form-control.input-monospace.input-sm.js-zeroclipboard-target.js-url-field').get_attribute("value")
        driver.save_screenshot('selenium-cloneurl.png')
        print(clone_url)
        assert (clone_url == "https://github.com/runningskull/gnugo.git")

    def test_random_file(self):
        driver = self.driver
        driver.get("https://github.com/runningskull/gnugo")
        with open("gnugo-file-list.txt", "r") as fileListFile:
            content = fileListFile.read()
            content = content.split("\n")
            originalFilePath = random.choice(content)
            print("random choose %s\n" % originalFilePath)
        filePath = originalFilePath.split("/")
        print(filePath)
        newFilePath = ""
        for i in range(2, len(filePath)):
            print(i)
            directory = driver.find_element_by_xpath('//*[@title="%s"]' % filePath[i])
            directory.click()
            newFilePath += filePath[i]+"/"
        driver.save_screenshot('selenium-file.png')
        raw_url = driver.find_element_by_id("raw-url").get_attribute("href")
        print(raw_url)

        rawfile = requests.get(raw_url)
        with open("tempcheckfile", "w") as tempfile:
            tempfile.write(rawfile.content)
        assert(filecmp.cmp("tempcheckfile", originalFilePath))

    def test_login(self):
        with open("github_account_passwd.txt", "r") as githubFile:
            content = githubFile.read()
            github_account = content.split('\n')[0]
            github_passwd = content.split('\n')[1]
        driver = self.driver
        driver.get("http://github.com/login")
        driver.find_element_by_id("login_field").clear()
        driver.find_element_by_id("login_field").send_keys(github_account)
        driver.find_element_by_id("password").clear()
        driver.find_element_by_id("password").send_keys(github_passwd)
        driver.find_element_by_name("commit").click()
        avatar = driver.find_element_by_css_selector(".avatar")
        driver.save_screenshot('selenium-login.png')
        assert(avatar.get_attribute("alt") == "@"+github_account)

    def test_switch_branch(self):
        driver = self.driver
        driver.get("https://github.com/runningskull/gnugo")
        
        #branchbutton.click()
        driver.find_element_by_xpath("//body").send_keys('w') # open branch select menu
        specificbranchbuttons = driver.find_elements_by_css_selector('.select-menu-item.js-navigation-item.js-navigation-open') # possible to have a lot branches
        driver.save_screenshot('selenium-switchbranch1.png')
        for button in specificbranchbuttons:
            if button.text == "origin":
                button.click()
        time.sleep(5) # wait for javascript complete
        driver.save_screenshot('selenium-switchbranch2.png')
        branchbutton = driver.find_element_by_css_selector('.btn.btn-sm.select-menu-button.js-menu-target.css-truncate')
        print(branchbutton)
        currentBranchName = branchbutton.find_element_by_tag_name("span").text
        print(currentBranchName)
        assert(currentBranchName == "origin")
    
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()
