def tableStart():
    Start = ''' 
\\begin{table}[htb!]
\\begin{center}
    '''
    return Start

def tabularStart(ncol):
    col = "cc"
    if ncol == 3: col = "ccc"
    elif ncol == 4: col = "cccc"
    elif ncol == 5: col = "ccccc"
    else: return ''
    
    tabStart = '''
\\setlength{\\tabcolsep}{0.0pc}{
\\small
\\begin{tabular*}{\\textwidth}{@{\\extracolsep{\\fill}}l%s}
\\noalign{\\smallskip}\\hline\\hline\\noalign{\\smallskip}
    ''' % (col)
    return tabStart

def tabularEnd():
    tabEnd = '''
\\noalign{\\smallskip}\\hline\\hline\\noalign{\\smallskip}
\\end{tabular*}
}
    '''
    return tabEnd

def tableEnd(label="observed_sr_yields_table"):
    End = '''
\\end{center}
\\caption{The number of observed data events and expected background contributions in the RPC and RPV signal regions for an integrated 
luminosity of \\SI{36.47}{fb^{-1}}. The ``Rare'' category contains the contributions from $t\\bar{t}h$, $t\\bar{t}+WW$, as well as $tZ$, $tWZ$, $Wh$, $Zh$, 
and triboson production. Background categories shown as ``$-$'' denote that they cannot contribute to a given region (charge-flip in 3-lepton regions).
The displayed yields include all sources of statistical and systematic uncertainties. The individual uncertainties can be correlated
and therefore do not necessarily add up in quadrature to the uncertainty on the total expected background.}
\\label{tab:%s}
\\end{table}
    ''' % (label)
    return End

def insertSingleLine():
    Line = '''
\\noalign{\\smallskip}\\hline\\noalign{\\smallskip}
    '''
    return Line

def insertDoubleLine():
    Line = '''
\\noalign{\\smallskip}\\hline\\hline\\noalign{\\smallskip}
    '''
    return Line

def insertSRnames(entries):
    Line = ''
    if len(entries)==2:
        Line = '''
        %s    &    %s    &    %s    \\\[-0.05cm]
        ''' % ('', entries[0],entries[1])
    elif len(entries)==3:
        Line = '''
        %s    &    %s    &    %s    &    %s    \\\[-0.05cm]
        ''' % ('', entries[0], entries[1], entries[2])
    elif len(entries)==4:
        Line = '''
        %s    &    %s    &    %s    &    %s    &    %s    \\\[-0.05cm]
        ''' % ('', entries[0], entries[1], entries[2], entries[3])
    elif len(entries)==5:
        Line = '''
        %s    &    %s    &    %s    &    %s    &    %s    &    %s    \\\[-0.05cm]
        ''' % ('', entries[0], entries[1], entries[2], entries[3], entries[4])
    return Line

def insertObserved(entries):
    Line = ''
    if len(entries)==2:
        Line = '''
        %s    &    %i    &    %i    \\\\
        ''' % ('Observed', entries[0],entries[1])
    elif len(entries)==3:
        Line = '''
        %s    &    %i    &    %i    &    %i    \\\\
        ''' % ('Observed', entries[0], entries[1], entries[2])
    elif len(entries)==4:
        Line = '''
        %s    &    %i    &    %i    &    %i    &    %i    \\\\
        ''' % ('Observed', entries[0], entries[1], entries[2], entries[3])
    elif len(entries)==5:
        Line = '''
        %s    &    %i    &    %i    &    %i    &    %i    &    %i    \\\\
        ''' % ('Observed', entries[0], entries[1], entries[2], entries[3], entries[4])
    return Line

def insertTotalSM(entries, errors):
    Line = ''
    if len(entries)==2:
        Line = '''
        %s    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    \\hspace*{1.95mm}  \\\\
        ''' % ('Total SM background', entries[0], errors[0], entries[1], errors[1])
    elif len(entries)==3:
        Line = '''
        %s    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    \\hspace*{1.95mm} \\\\  
        ''' % ('Total SM  background', entries[0], errors[0], entries[1], errors[1], entries[2], errors[2])
    elif len(entries)==4:
        Line = '''
        %s    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    \\hspace*{1.95mm}  \\\\ 
        ''' % ('Total SM background', entries[0], errors[0], entries[1], errors[1], entries[2], errors[2], entries[3], errors[3])
    elif len(entries)==5:
        Line = '''
        %s    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &   %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    \hspace*{1.95mm}  \\\\
        ''' % ('Total SM background', entries[0], errors[0], entries[1], errors[1], entries[2], errors[2], entries[3], errors[3], entries[4], errors[4])
    return Line


def insertBkg(name, entries, errors):
    Line = ''
    if len(entries)==2:
        Line = '''
        %s    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    \\\\
        ''' % (name, entries[0], errors[0], entries[1], errors[1])
    elif len(entries)==3:
        Line = '''
        %s    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    \\\\  
        ''' % (name, entries[0], errors[0], entries[1], errors[1], entries[2], errors[2])
    elif len(entries)==4:
        Line = '''
        %s    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    \\\\ 
        ''' % (name, entries[0], errors[0], entries[1], errors[1], entries[2], errors[2], entries[3], errors[3])
    elif len(entries)==5:
        Line = '''
        %s    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    &   %.2f $\\pm$ %.2f    &    %.2f $\\pm$ %.2f    \\\\
        ''' % (name, entries[0], errors[0], entries[1], errors[1], entries[2], errors[2], entries[3], errors[3], entries[4], errors[4])
    return Line



