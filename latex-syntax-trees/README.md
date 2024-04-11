# Beautiful Syntax Trees in LaTeX

I used `tikz-qtree` to generate these syntax trees for my syntactic analysis
course. Full PDF [here](syntax-trees.pdf).

``` tex
\usepackage{tikz-qtree,tikz-qtree-compat}
\newcommand{\qtl}[1]{\strut #1} % align to baseline
```

## Samples and Code
<center><img src=x-bar.png></center>

``` tex
\begin{tikzpicture}
  \tikzset{every tree node/.style={scale=1.0}}
  \tikzset{every leaf node/.style={scale=0.9,text=orange}}
  \tikzset{frontier/.style={level distance=18pt}}

  \Tree [.TP
    [.NP [.N'
      [.N' [.N \edge[draw=none]; \qtl{Armadillos} ]]
      [.PP [.P'
        [.P \edge[draw=none]; \qtl{from} ]
        [.NP [.N' [.N \edge[draw=none]; \qtl{New York} ]]]]]]]
    [.VP [.V'
      [.V'
        [.AdvP [.Adv' [.Adv \edge[draw=none]; \qtl{often} ]]]
        [.V'
	  [.V \edge[draw=none]; \qtl{destroy} ]
	  [.NP [.N'
            [.AdjP [.Adj' [.Adj \edge[draw=none]; \qtl{old} ]]]
            [.N' [.N \edge[draw=none]; \qtl{pillowcases} ]]]]]]
      [.PP [.P'
        [.P \edge[draw=none]; \qtl{with} ]
        [.NP
	  [.D \edge[draw=none]; \qtl{their} ]
          [.N' [.N \edge[draw=none]; \qtl{snouts} ]]]]]]]]
\end{tikzpicture}
```


<center><img src=binding-domain.png></center>

``` tex
\newcommand{\npi}[1][i]{\textsubscript{#1}}

\begin{tikzpicture}
  \tikzset{every tree node/.style={scale=0.8}}
  \tikzset{every leaf node/.style={scale=0.9,font=\itshape}}
  \tikzset{interior/.style={level distance=20pt}}
  \tikzset{frontier/.style={distance from root=70pt}}

  \Tree [.TP
    [.NP\npi[j]
      [.NP\npi{} [.N \edge[draw=none]; \qtl{Michael's} ]]
      [.N \edge[draw=none]; \qtl{father} ]]
    [.VP
      [.V \edge[draw=none]; \qtl{loves} ]
      [.\textcolor{red}{NP\npi{}}
        [.N \edge[draw=none]; \qtl{\textcolor{red}{himself}} ]]]]
\end{tikzpicture}
```

<center><img src=complements-adjuncts.png></center>

``` tex
\begin{tikzpicture}
  \tikzset{every tree node/.style={scale=0.8}}
  \tikzset{every leaf node/.style={scale=0.9,font=\itshape}}
  \tikzset{interior/.style={level distance=16pt}}
  \tikzset{frontier/.style={level distance=20pt}}

  \Tree [.NP
    [.D \edge[draw=none]; \qtl{The} ]
    [.N'
      [.N'
        [.N \edge[draw=none]; \qtl{collection} ]
        [.PP \edge[roof]; \qtl{of figurines} ]]
      [.PP \edge[roof]; \qtl{in the window} ]]]
\end{tikzpicture}
```