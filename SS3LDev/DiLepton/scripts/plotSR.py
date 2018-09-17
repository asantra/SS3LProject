import os
import sys
import time
import ROOT
import pickle
import SRTableTex
from math import sqrt
from glob import glob
from argparse import ArgumentParser

def main():
    parser = ArgumentParser(description="Script for producing SR summary plot")
    parser.add_argument("--InDir",  help="Directory with yields tables",    default="")
    parser.add_argument("--Blind",  type=int, help="Observed = Expected",   default=0)
    parser.add_argument("--Plot",   type=int, help="Creating plot with SR", default=1)
    parser.add_argument("--fitted", type=int, help="Use post-fit numbers",  default=1)
    parser.add_argument("--makeTex",type=int, help="Creating .tex output",  default=0)
    parser.add_argument("--Name",   help="Name of output plot",   default="SRsummary")
    parser.add_argument("--Ratio",  type=int, help="Plot Nobs/Nexp",        default=1)  
    parser.add_argument("--LogY",   type=int, help="Using y-axis in log",   default=1) 
    options = parser.parse_args()

    if not len(options.InDir) or not os.path.exists(options.InDir):
        print red("Please specify an input directory"); sys.exit(1)
    
    SRfiles = sortedList(glob(options.InDir+"/YieldsBgOnly*.pickle"))
    if not len(SRfiles):
        print red("No SR files found"); sys.exit(1)

    print bold("-- Looking in {0}: Found {1} SR files".format(options.InDir,len(SRfiles)))
    print bold("-- Running in blind mode {0}".format(options.Blind))
    print bold("-- Using post-fit numbers {0}".format(options.fitted))
    
    SRNames = []; Nexp = []; NexpErr = []; Nobs = []; NobsErr = []; NBkg = []; EBkg = [];
    for SRfile in SRfiles:
        try: fYield = open(SRfile,"r")
        except:
            print red("{0} cannot be opened".format(SRfile)); continue

        SRname = SRName(SRfile)
        SRNames.append( SRname )
        
        SRYields = pickle.load(fYield)
        exp  = SRYields[keyBkgTot(options.fitted)][0] 
        expE = SRYields[keyErrTot(options.fitted)][0] 
    
        expE = addStats(expE, SRname, "TOTAL")
        expE = ROOT.TMath.Min(exp, expE)

        obs  = observed(SRname) if not options.Blind else exp

        Nexp.append( float(exp) )
        Nobs.append( float(obs) )
        NexpErr.append( float(expE) )

        nbkg = []; ebkg = [];
        for i in range(len(bkgList())):
            nB = SRYields[keyBkg(options.fitted)+bkgList()[i]][0]
            eB = SRYields[keyErr(options.fitted)+bkgList()[i]][0] 
        
            eB = addStats(eB, SRname, bkgList()[i])
            eB = ROOT.TMath.Min(nB, eB)

            nbkg.append( float(nB) )
            ebkg.append( float(eB) )
        NBkg.append(nbkg)
        EBkg.append(ebkg)
            
        print "--> {0}, Nexp={1:.2f} +/- {2:.2f}, Nobs={3:.2f} +/- {4:.2f}".format(SRname, exp, expE, obs, sqrt(float(obs)))
        for i in range(len(bkgList())):
            print "    {0} {1:.2f} +/- {2:.2f}  ".format(bkgList()[i],nbkg[i],ebkg[i]),
        print

    print bold("-- Retrieved numbers from {0} SR".format(len(Nexp)))
    if options.makeTex:
        makeTex(options.Name, SRNames, Nexp, NexpErr, Nobs, NBkg, EBkg)    

    if options.Plot:
        makePlot(options.Name, SRNames, Nexp, NexpErr, Nobs, NBkg, options.LogY, options.Ratio)
        time.sleep(2)
    return

def bkgList():
    bkg = ["Rare", "FourTop", "ttH", "ttV", "Diboson", "Fakes", "MisCharge"]
    return bkg

def makeTex(name, srnames, nexp, err_exp, nobs, nbkg, ebkg):

    texFile = open("{0}.tex".format(name),"w")

    tabLabel1 = "observed_sr_yields_table1"
    texFile.write( SRTableTex.tableStart() )
    texFile.write( SRTableTex.tabularStart(5) )
    texFile.write( SRTableTex.insertSRnames(srnames[0:5]) )
    texFile.write( SRTableTex.insertSingleLine() )
    texFile.write( SRTableTex.insertObserved(nobs[0:5]) )
    texFile.write( SRTableTex.insertDoubleLine() )
    texFile.write( SRTableTex.insertTotalSM(nexp[0:5],err_exp[0:5]) )
    texFile.write( SRTableTex.insertSingleLine() )
    for i in range(len(bkgList())):
        texFile.write( SRTableTex.insertBkg( texName(bkgList()[i]), [b[i] for b in nbkg[0:5]], [e[i] for e in ebkg[0:5]] ) )
    texFile.write( SRTableTex.tabularEnd() )
    texFile.write( SRTableTex.tabularStart(5) )
    texFile.write( SRTableTex.insertSRnames(srnames[5:10]) )
    texFile.write( SRTableTex.insertSingleLine() )
    texFile.write( SRTableTex.insertObserved(nobs[5:10]) )
    texFile.write( SRTableTex.insertDoubleLine() )
    texFile.write( SRTableTex.insertTotalSM(nexp[5:10],err_exp[5:10]) )
    texFile.write( SRTableTex.insertSingleLine() )
    for i in range(len(bkgList())):
        texFile.write( SRTableTex.insertBkg( texName(bkgList()[i]), [b[i] for b in nbkg[5:10]], [e[i] for e in ebkg[5:10]] ) )
    texFile.write( SRTableTex.tabularEnd() )
    texFile.write( SRTableTex.tableEnd(tabLabel1) )

    tabLabel2 = "observed_sr_yields_table2"
    texFile.write( SRTableTex.tableStart() )
    texFile.write( SRTableTex.tabularStart(5) )
    texFile.write( SRTableTex.insertSRnames(srnames[10:15]) )
    texFile.write( SRTableTex.insertSingleLine() )
    texFile.write( SRTableTex.insertObserved(nobs[10:15]) )
    texFile.write( SRTableTex.insertDoubleLine() )
    texFile.write( SRTableTex.insertTotalSM(nexp[10:15],err_exp[10:15]) )
    texFile.write( SRTableTex.insertSingleLine() )
    for i in range(len(bkgList())):
        texFile.write( SRTableTex.insertBkg( texName(bkgList()[i]), [b[i] for b in nbkg[10:15]], [e[i] for e in ebkg[10:15]] ) )
    texFile.write( SRTableTex.tabularEnd() )
    texFile.write( SRTableTex.tabularStart(4) )
    texFile.write( SRTableTex.insertSRnames(srnames[15:19]) )
    texFile.write( SRTableTex.insertSingleLine() )
    texFile.write( SRTableTex.insertObserved(nobs[15:19]) )
    texFile.write( SRTableTex.insertDoubleLine() )
    texFile.write( SRTableTex.insertTotalSM(nexp[15:19],err_exp[15:19]) )
    texFile.write( SRTableTex.insertSingleLine() )
    for i in range(len(bkgList())):
        texFile.write( SRTableTex.insertBkg( texName(bkgList()[i]), [b[i] for b in nbkg[15:19]], [e[i] for e in ebkg[15:19]] ) )
    texFile.write( SRTableTex.tabularEnd() )
    texFile.write( SRTableTex.tableEnd(tabLabel2) )
    
    print bold("-- Created {0}".format(texFile.name))
    return

def makePlot(name, srnames, nexp, err_exp, nobs, nbkg, logy, ratio):    

    AtlasStyle = "/home/fc12/Atlas/atlasstyle-00-03-05/"
    if not os.path.exists(AtlasStyle):
        print red("{0} not found. Please set path for AtlasStyle".format(AtlasStyle)); return

    ROOT.gROOT.LoadMacro(AtlasStyle+"AtlasStyle.C")
    ROOT.gROOT.ProcessLine("SetAtlasStyle()")
    
    NSR = len(nexp)
    print green("Creating TH1F with {0} entries".format(NSR))

    hData = ROOT.TH1F("hData","hData", NSR, 0, NSR)
    hTot  = ROOT.TH1F("hTot", "hTot",  NSR, 0, NSR)

    hBkg  = ROOT.THStack("hBkg", "hBkg")

    bkgHist = []
    for i in range(len(bkgList())):
        hbkg = ROOT.TH1F(bkgList()[i],bkgList()[i], NSR, 0, NSR)
        bkgHist.append(hbkg)


    for sr in range(NSR):
        hTot.SetBinContent(sr+1, nexp[sr])
        hTot.SetBinError(sr+1, err_exp[sr])

        hData.SetBinContent(sr+1, nobs[sr])
        hData.SetBinError(sr+1, 0.01)

        for i in range(len(bkgList())):
            bkgHist[i].SetBinContent(sr+1, nbkg[sr][i])

    for i in range(len(bkgList())):
        bkgHist[i].SetLineWidth(1)
        bkgHist[i].SetLineColor(ROOT.kBlack)
        setBkgColor(bkgHist[i])
        hBkg.Add(bkgHist[i])
        
    if not ratio:
        c  = ROOT.TCanvas(name, name, 1, 10, 1150, 480)
        p1 = ROOT.TPad(name+"1",name+"1", 0.00, 0.00, 1.00, 1.00, -1, 0, 0)
        c.cd()
        p1.SetLeftMargin(0.07)  
        p1.Draw()
    
    else:
        c  = ROOT.TCanvas(name, name, 1, 10, 1150, 560)
        p1 = ROOT.TPad(name+"1",name+"1", 0.00, 0.20, 1.00, 1.00, -1, 0, 0)
        p2 = ROOT.TPad(name+"2",name+"2", 0.00, 0.00, 1.00, 0.32, -1, 0, 0)
        c.cd()
        p1.SetLeftMargin(0.07)
        p2.SetLeftMargin(0.07)
        p2.SetBottomMargin(0.4) 
        p1.Draw()
        p2.Draw()
            
    if logy: p1.SetLogy()
    p1.SetGridx()
    p1.cd()

    yMax = ROOT.TMath.Max(hData.GetMaximum(), hTot.GetMaximum())
    if logy:
        hData.GetYaxis().SetRangeUser(0.021, yMax*25.)
    else:
        hData.GetYaxis().SetRangeUser(0.021, yMax*1.8)

    for i in range(len(srnames)):
        hData.GetXaxis().SetBinLabel(i+1,srnames[i])

    setDataStyle(hData)
    hData.Draw("E0")
    gData = makeGraph(hData)
    setDataStyle(gData)
    gData.Draw("SAME P0")

    setMCStyle(hTot, "EX")
    hBkg.Draw("SAME HIST")
    hTot.Draw("SAME E2")

    gDataW = makeWhiteCopy(gData)
    gDataW.Draw("SAME P0")
    gData.Draw("SAME P0")

    leg = makeLegend(3)
    leg.AddEntry(hData,"Data",    "pl")
    leg.AddEntry(hTot, "SM Total","f")
    for i in range(len(bkgList())):
        leg.AddEntry(bkgHist[i], legendName(bkgList()[i]),"f")
    leg.Draw("same")
  
    drawATLASlabel(0.10,0.83,"ATLAS","Internal",36.1)
    ROOT.gPad.RedrawAxis()

    if ratio:
        p2.cd()
        p2.SetGridx()
        p2.SetGridy()

        hRatio = makeRatioPlotHisto(hData,hTot)
        setRatioStyle(hRatio,0.3,1.8)
        hRatio.Draw("E0")
        gRatio = makeRatioPlotGraph(gData,hTot)
        setRatioStyle(gRatio,0.3,1.8)
        gRatio.Draw("P0 SAME")

        hRatioErrEX = makeRatioError(hTot)
        hRatioErrTH = makeRatioError(hTot)
        setMCStyle(hRatioErrEX, "EX")
        setMCStyle(hRatioErrTH, "TH")
        hRatioErrTH.Draw("SAME E2")
        hRatioErrEX.Draw("SAME E2")
        gRatio.Draw("SAME P0")

        setLine(NSR)
        ROOT.gPad.SetTicks()
        ROOT.gPad.RedrawAxis()
    
    c.Print("{0}.pdf".format(name))
    print bold("-- Created {0}.pdf".format(name))
    return

def setBkgColor(histo):
    if (histo.GetName()).find("ttV")>-1:       histo.SetFillColor(ROOT.kViolet-9); return
    if (histo.GetName()).find("ttH")>-1:       histo.SetFillColor(ROOT.kCyan-3);   return
    if (histo.GetName()).find("Diboson")>-1:   histo.SetFillColor(ROOT.kAzure-2);  return
    if (histo.GetName()).find("FourTop")>-1:      histo.SetFillColor(ROOT.kOrange+7);  return
    if (histo.GetName()).find("Rare")>-1:      histo.SetFillColor(ROOT.kGreen+2);  return
    if (histo.GetName()).find("MisCharge")>-1: histo.SetFillColor(ROOT.kRed+3);    return
    if (histo.GetName()).find("Fakes")>-1:     histo.SetFillColor(ROOT.kOrange-9); return
    return

def makeLegend(col):
    l = ROOT.TLegend(0.66, 0.76, 0.96, 0.92)
    l.SetNColumns(col)
    l.SetBorderSize(0)
    l.SetTextFont(42)
    l.SetTextSize(0.03)
    l.SetFillColor(0)
    l.SetFillStyle(0)
    l.SetLineColor(0)
    return l

def legendName(name):
    l = {"ttV"       : "t#bar{t}+V", 
         "ttH"       : "t#bar{t}+H", 
         "Rare"      : "Rare", 
         "FourTop"   : "4t",
         "Diboson"   : "Diboson", 
         "MisCharge" : "Charge-flip", 
         "Fakes"     : "Fakes"}
    return l[name]

def texName(name):
    l = {"ttV"       : "$t\\bar{t}+V$",
         "ttH"       : "$t\\bar{t}+H$",
         "Rare"      : "Rare",
         "FourTop"   : "4t",
         "Diboson"   : "Diboson",
         "MisCharge" : "Charge-flip",
         "Fakes"     : "Fake/non-prompt"}
    return l[name]
    

def SRName(name):
    srname = name.split("/")[-1].split(".pickle")[0].replace("YieldsBgOnly","")
    return srname
                
def observed(sr):
    obs = {"Rpc2L0bH" : 3,
           "Rpc2L0bS" : 7,
           "Rpc2L1bH" : 13,
           "Rpc2L1bS" : 14,
           "Rpc2L2bH" : 0,
           "Rpc2L2bS" : 3,
           "Rpc2Lsoft1b" : 4,
           "Rpc2Lsoft2b" : 5,
           "Rpc3L0bH"  : 3,
           "Rpc3L0bS"  : 9,
           "Rpc3L1bH"  : 4,
           "Rpc3L1bS"  : 20,
           "Rpc3LSS1b" : 1,
           "Rpv2L0b"   : 2,
           "Rpv2L1bH"  : 2,
           "Rpv2L1bM"  : 9,
           "Rpv2L1bS"  : 26,
           "Rpv2L2bH"  : 1,
           "Rpv2L2bS"  : 20}    
    return obs[sr]

def addStats(error, srname, bname, msg=False):
    if msg: print "addStats :: SR={0} \t Bkg={1} \t SysErr={2}".format(srname, bname, error)

    MCStats = "/home/fc12/HistFitter2017/SSHistHitter/Tables/MCstats.txt"
    if not os.path.exists(MCStats):
        print red("{0} not found. Returning original error".format(MCStats)); return error

    bkgs = []
    if bname == "TOTAL":     bkgs = ["ttW", "ttZ", "ttH", "FourTop", "WW", "WZ", "ZZ", "Rare", "Fakes", "MisCharge"]
    if bname == "ttV":       bkgs = ["ttZ", "ttW"]
    if bname == "Diboson":   bkgs = ["WW", "WZ", "ZZ"]
    if bname == "ttH":       bkgs = ["ttH"]
    if bname == "Rare":      bkgs = ["Rare"]
    if bname == "FourTop":   bkgs = ["FourTop"]
    if bname == "Fakes":     bkgs = ["Fakes"]
    if bname == "MisCharge": bkgs = ["MisCharge"]

    statErr = [] 
    f = open(MCStats, "r")
    for line in f:
        line = line.rstrip('\n\r')
        l = line.split(',')
        bkg = l[0].replace(" ","")
        sr  = l[1].replace(" ","")
        err = float(l[2])
        if bkg in bkgs and sr==srname: 
            if msg: print "--> Found Stat({0})={1}".format(l[0],l[2]) 
            statErr.append(err*err)

    totErr = error*error
    for i in range(len(statErr)): totErr += statErr[i]
    totErr = ROOT.TMath.Sqrt(totErr)

    if msg: print "addStats :: Updated error {0}".format(totErr)
    return totErr

def setDataStyle(h):
    h.GetXaxis().SetLabelSize(0.05)
    h.GetXaxis().SetLabelOffset(0.2)
    h.GetYaxis().SetLabelSize(0.04)
    h.GetYaxis().SetTitle("Events")
    h.GetYaxis().SetTitleSize(0.04)
    h.GetYaxis().SetTitleOffset(0.7)
    h.SetMarkerColor(ROOT.kBlack)
    h.SetLineColor(ROOT.kBlack)
    h.SetMarkerSize(0.8)
    h.SetLineWidth(1)
    return

def setMCStyle(h,Type):
    if Type=="EX":
        h.SetLineColor(ROOT.kGray+1)
        h.SetFillColor(ROOT.kGray+1)
        h.SetFillStyle(3254)
    if Type=="TH":
        h.SetLineColor(ROOT.kYellow-9)
        h.SetFillColor(ROOT.kYellow-9)
        h.SetFillStyle(1111)
    h.SetLineWidth(1)
    h.SetMarkerSize(0)
    return

def setRatioStyle(h,ymin,ymax):
    h.GetXaxis().SetLabelSize(0.14)
    h.GetXaxis().SetLabelOffset(0.04)
    h.GetYaxis().SetLabelSize(0.1)
    h.GetYaxis().SetTitle("Data/SM")
    h.GetYaxis().SetTitleSize(0.1)
    h.GetYaxis().SetTitleOffset(0.27)
    h.GetYaxis().SetRangeUser(ymin,ymax)
    h.GetYaxis().SetNdivisions(6)
    h.SetMarkerColor(ROOT.kBlack)
    h.SetLineColor(ROOT.kBlack)
    h.SetMarkerSize(0.8)
    h.SetLineWidth(1)
    return
                                            

def makeWhiteCopy(h):
    copy = h.Clone(h.GetName()+"_white")
    copy.SetLineColor(ROOT.kWhite)
    copy.SetMarkerColor(ROOT.kWhite)
    copy.SetMarkerSize(1.1)
    copy.SetLineWidth(2)
    return copy

def makeRatioPlotHisto(h1, h2):
    h = h1.Clone("Ratio_"+h1.GetName()+"_"+h2.GetName())
    h.Reset()
    for i in range(h.GetNbinsX()):
        r = h1.GetBinContent(i+1) / h2.GetBinContent(i+1) if h2.GetBinContent(i+1)>0 else 0
        e = h1.GetBinError(i+1) / h1.GetBinContent(i+1) if h1.GetBinContent(i+1)>0 else 0
        h.SetBinContent(i+1,r)
        h.SetBinError(i+1,e)
    return h


def makeRatioPlotGraph(g, h):
    gR = ROOT.TGraphAsymmErrors()
    gR.SetName("Ratio_{0}_{1}".format(g.GetName(),h.GetName()))
    for i in range(g.GetN()):
        if not h.GetBinContent(i+1): continue
        x = ROOT.Double(0.) 
        y = ROOT.Double(0.)
        g.GetPoint(i, x, y)

        r  = y / h.GetBinContent(i+1) 
        eL = g.GetErrorYlow(i) / h.GetBinContent(i+1)
        eU = g.GetErrorYhigh(i) / h.GetBinContent(i+1)

        gR.SetPoint(i, x, r)
        gR.SetPointError(i, g.GetErrorXlow(i), g.GetErrorXhigh(i), eL, eU)
    return gR

def makeRatioError(h):
    hErr = h.Clone("RatioErr_"+h.GetName())
    hErr.Reset()
    for i in range(hErr.GetNbinsX()):
        e = h.GetBinError(i+1) / h.GetBinContent(i+1) if h.GetBinContent(i+1)>0 else 0
        hErr.SetBinContent(i+1,1)
        hErr.SetBinError(i+1,e)
    return hErr

def setLine(xmax):
    line = ROOT.TLine()
    line.SetLineColor(ROOT.kBlue+1)
    line.SetLineStyle(9)
    line.SetLineWidth(1)
    line.DrawLine(0, 1., xmax, 1.)
    return

def makeGraph(h):
    g = ROOT.TGraphAsymmErrors()
    g.SetName("Graph_{0}".format(h.GetName()))
    for i in range(h.GetNbinsX()):
        x = h.GetBinCenter(i+1)
        y = h.GetBinContent(i+1)
        eL = ROOT.TMath.Abs(poissonCLlow(y) - y)
        eU = ROOT.TMath.Abs(poissonCLup(y) - y)

        g.SetPoint(i, x, y)
        g.SetPointError(i, h.GetBinWidth(i+1)/2., h.GetBinWidth(i+1)/2., eL, eU)
        print "--> DataPoint({0}) :: Obs={1:.1f}  \t errLow={2:.3f}, errUp={3:.3f}".format(i, y, eL, eU)
    return g

def drawATLASlabel(x,y,label1,label2,lumi):
    n = ROOT.TLatex()
    n.SetNDC()
    n.SetTextColor(ROOT.kBlack)
    n.SetTextFont(72)
    n.SetTextSize(0.04)
    n.DrawLatex(x,y,label1)
    n.SetTextFont(62)
    n.DrawLatex(x+0.055,y,label2)
    n.SetTextFont(42)
    n.DrawLatex(x+0.12,y,"#sqrt{s} = 13 TeV, %s fb^{-1}" % (lumi))
    return


def poissonCLlow(obs, q=0.68):
    LL = 0.
    if obs >= 0.001:
        a = (1. - q) / 2. 
        LL = ROOT.TMath.ChisquareQuantile(a, 2.*obs) / 2.
    return LL

def poissonCLup(obs, q=0.68):
    UL = 0.
    if obs >= 0.001: 
        a = 1. - (1. - q) / 2.
        UL = ROOT.TMath.ChisquareQuantile(a, 2.*(obs+1.)) / 2.
    return UL

def sortedList(inlist, dosort=True):

    if not dosort: return inlist 
    outlist = []
    tmplist = ["Rpc2L2bS",
               "Rpc2L2bH",
               "Rpc2Lsoft1b",
               "Rpc2Lsoft2b",
               "Rpc2L0bS",
               "Rpc2L0bH",
               "Rpc3L0bS",
               "Rpc3L0bH",
               "Rpc3L1bS",
               "Rpc3L1bH",
               "Rpc2L1bS",
               "Rpc2L1bH",
               "Rpc3LSS1b",
               "Rpv2L1bH",
               "Rpv2L0b",
               "Rpv2L2bH",
               "Rpv2L2bS",                
               "Rpv2L1bS",
               "Rpv2L1bM"]

    for i in range(len(tmplist)):
        for j in range(len(inlist)):
            if inlist[j].find(tmplist[i])>-1: outlist.append( inlist[j] )

    print bold("-- Sorted SR list with {0} entries".format(len(outlist)))
    return outlist


def keyBkgTot(f): 
    if f: return 'TOTAL_FITTED_bkg_events'
    else: return 'TOTAL_MC_EXP_BKG_events'
 
def keyErrTot(f): 
   if f: return 'TOTAL_FITTED_bkg_events_err'
   else: return 'TOTAL_MC_EXP_BKG_err'

def keyBkg(f):
    if f: return 'Fitted_events_'
    else: return 'MC_exp_events_'

def keyErr(f):
    if f: return 'Fitted_err_'
    else: return 'MC_exp_err_'
    
def green(msg): return "\033[0;32m{0}\033[0m".format(msg);
def red(msg): return "\033[0;31m{0}\033[0m".format(msg);
def bold(msg): return "\033[1m{0}\033[0m".format(msg);

    
if __name__ == "__main__":
    main()

