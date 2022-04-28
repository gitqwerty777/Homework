(TeX-add-style-hook
 "graph-hw5"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("algorithm" "plain") ("CJK" "encapsulated")))
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art10"
    "fancyhdr"
    "extramarks"
    "amsmath"
    "amsthm"
    "amsfonts"
    "tikz"
    "algorithm"
    "algpseudocode"
    "CJK"
    "graphicx"
    "caption"
    "subcaption")
   (TeX-add-symbols
    '("pderiv" 2)
    '("deriv" 1)
    '("alg" 1)
    '("exitProblemHeader" 1)
    '("enterProblemHeader" 1)
    "hmwkTitle"
    "hmwkClass"
    "hmwkClassTime"
    "hmwkClassInstructor"
    "hmwkAuthorName"
    "dx"
    "solution"
    "E"
    "Var"
    "Cov"
    "Bias")
   (LaTeX-add-environments
    '("homeworkProblem" LaTeX-env-args ["argument"] 0))
   (LaTeX-add-counters
    "partCounter"
    "homeworkProblemCounter")))

