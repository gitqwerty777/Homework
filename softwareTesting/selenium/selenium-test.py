with open("github_account_passwd.txt", "r") as githubFile:
    content = githubFile.read()
    github_account = content.split('\n')[0]
    github_passwd = content.split('\n')[1]


