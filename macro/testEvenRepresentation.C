{
  double y[512]; 
  for (int i = 0; i < 512; i++) y[i] = i*i % 32 - 16; 

  TCanvas * c = new TCanvas; 
  c->Divide(2,1); 

  c->cd(1); 
  nurfana::EvenRepresentation w(512,y,0.5); 
  w.Draw(); 

  c->cd(2); 
  nurfana::FrequencyRepresentation f(w); 
  f.Draw(); 
}
