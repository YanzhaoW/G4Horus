error_ppg = function(.x, .y, .dx, .dy, f){
  vars=c("x", "y", "dx", "dy")
  exprs = list(
    # expression to compute new variable values
    deparse(f[[2]]),
    
    # expression to compute new variable errors
    sapply(vars[c(1,2)], function(v) {
      dfdp = deparse(D(f[[2]], v))
      sprintf('(d%s*(%s))^2', v, dfdp)
    }) %>%
      paste(collapse='+') %>%
      sprintf('sqrt(%s)', .)
  )
  body1<-exprs[[1]]
  body2<-exprs[[2]]
  eval(parse(text = paste('fun <- function(x,y) { return(' , body1 , ')}', sep='')))
  eval(parse(text = paste('dfun <- function(x,y,dx,dy) { return(' , body2 , ')}', sep='')))
  ans<- matrix(c(fun(.x, .y), dfun(.x, .y, .dx, .dy)), ncol=2, dimnames=list(NULL,c("z","dz")))
  return(ans)
}

library(ggplot2)
library(plyr)
library(dplyr)
library(tidyr)
library(magrittr)
library(ggpubr)
library(data.table)
data<-read.csv("efficiencies.csv",na.strings = "NA")
det <- c("A0","A1","A2","A3","B0","B1","B2","B3")
data_cs<-filter(data, Nuclide == "137Cs") %>% select(x100mm,dx100mm,x13mm,dx13mm)
geo_cor<-error_ppg(data_cs$x100mm, data_cs$x13mm, data_cs$dx100mm, data_cs$dx13mm, ~y/x)
rownames(geo_cor)=det
data_cas<-filter(data, Nuclide == "152Eu" | Nuclide == "226Ra")
require(data.table)
setDT(data_cas)
setkey(data_cas,Energy,Detector)
dataName = names(data_cas)

for(de in det){
  fDet<-filter(data_cas, Detector==de)
  fData<-error_ppg(fDet$x100mm, geo_cor[de, "z"], fDet$dx100mm,geo_cor[de, "dz"], ~x*y)
  fData<-error_ppg(fData[,"z"], fDet$x13mm, fData[,"dz"],fDet$dx13mm, ~x/y)
  data_cas[J(fDet$Energy,de),':='(z=fData[,1],dz=fData[,2])]
}
data_cas<-arrange(data_cas, Energy)
write.csv(x=data_cas, file ="SummingCorrectionFactor.csv", quote=FALSE, na="NA", row.names=FALSE)

nuclide="226Ra"
colorE<-c("black","red","blue","green","cyan","darkviolet","brown")
gplot<-data_cas%>%drop_na()%>%filter(Nuclide==nuclide)%>%
  ggplot(aes(x=Energy, y=z, group=Detector,color=Detector))+
  geom_line()+
  geom_errorbar(mapping=aes(ymin=z-dz, ymax=z+dz), width=15, size=0.3)+
  scale_color_manual(name="detector",values = colorE)+
  labs(title=nuclide, x="Energy (keV)", y="summing correction factor")+
  ylim(0.7,1.3)+
  scale_x_continuous(breaks = scales::pretty_breaks(n = 15))+
  theme(legend.justification = c(1, 1), 
        legend.position = c(0.2, 0.45))+
  guides(fill=guide_legend(ncol=2))+
  guides(col=guide_legend(ncol=2))

ggsave(filename=paste0("SummingCorrectionFactor_",nuclide,".png",seq=''),plot=gplot,device =
         "png", path="./", height =4, width=8, units="in",dpi=500)



# energies<-levels(data_cas$Energy)
# sum_cor<-data.frame(matrix(NA, nrow=length(energies), ncol = length(det), dimnames = list(energies, det)))
# sum_cor_error<-data.frame(matrix(NA, nrow=length(energies), ncol = length(det), dimnames = list(energies, paste0(det,"_err",seq=''))))
# for(e in energies){
#   lev<-filter(data_cas, Energy==e)
#   true_eff<-error_ppg(lev$x100mm, geo_cor[, "z"], lev$dx100mm,geo_cor[, "dz"], ~x*y)
#   sum_cor[e, ]<-error_ppg(true_eff[,"z"], lev$x13mm, true_eff[,"dz"],lev$dx13mm, ~x/y)[,1]
#   sum_cor_error[e, ]<-error_ppg(true_eff[,"z"], lev$x13mm, true_eff[,"dz"],lev$dx13mm, ~x/y)[,2]
# }
# sum_cor_fac<-data.frame(matrix(NA, nrow=length(energies), ncol = 2*length(det), dimnames = list(energies, rep(0, 2*length(det)))))
# for(i in 1:length(det)){
#   sum_cor_fac[,2*i-1] = sum_cor[,i]
#   colnames(sum_cor_fac)[2*i-1]= colnames(sum_cor)[i]
#   sum_cor_fac[,2*i] = sum_cor_error[,i]
#   colnames(sum_cor_fac)[2*i]= colnames(sum_cor_error)[i]
# }
# sum_cor_fac <- tibble::rownames_to_column(sum_cor_fac, "Energy")
# 
# write.csv(x=sum_cor_fac, file = "SummingCorrectionFactor.csv", quote=FALSE, na="NA")
# sum_cor_fac$Energy<-as.numeric(sum_cor_fac$Energy)
# colorE<-c("black","red","blue","green","cyan","darkviolet","brown")
# plot<-ggplot(data=sum_cor_fac, aes(x=Energy))+
#   labs(x="Energy (keV)", y="summing correction factor")+
#   scale_x_continuous(breaks = scales::pretty_breaks(n = 15))
# 
# for(i in 2:length(det)){
#   plot<-plot+geom_line(aes_string(y=det[i],color=shQuote(det[i])))+
#     geom_errorbar(mapping=aes_string(ymin=paste0( det[i],'-',det[i],"_err",seq=''),
#                       ymax=paste0( det[i],'+',det[i],"_err",seq='')),color=colorE[i-1])
# }
# 
# plot<-plot+
#   theme(legend.justification = c(1, 1), 
#         legend.position = c(0.2, 0.45))+
#   guides(fill=guide_legend(ncol=2))+
#   scale_color_manual(name="detector",values = colorE[1:7],labels=det[2:8])+
#   guides(col=guide_legend(ncol=2))
# plot
# ggsave(filename="summing_factor.png",plot=plot,device = "png", path="./", height =4, width=8, units="in",dpi=500)
