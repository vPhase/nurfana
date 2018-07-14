{
  TF2 f("f","sin(x) +cos(y) + x*y",0,1,0,1);
  TH2 * h = (TH2*) f.CreateHistogram();
  nurfana::GridInterpolator2D grid(*h);
  TH2 * finer = new TH2D("finer","finer",1000,0,1,1000,0,1); 
  grid.evalHist(finer); 
  finer->SetContour(99); 
  finer->Draw("colz"); 

}

