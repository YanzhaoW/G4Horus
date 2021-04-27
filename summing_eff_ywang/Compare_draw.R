#!/usr/bin/ Rscript

suppressMessages(library(tidyr))
suppressMessages(library(dplyr))
suppressMessages(library(ggplot2))

args = commandArgs(trailingOnly=TRUE)

if (length(args)==0) {
  stop("Please input filesnames, 1st: Measured datafile. 2nd: sim datafile (by default factors.csv)", call.=FALSE)
} else if (length(args)==1) {
  # default output file
  expDataFile<-args[1]
  simDataFile<-"factors.csv"
}else if (length(args)==2) {
  # default output file
  expDataFile<-args[1]
  simDataFile<-args[2]
}else{
  stop("Please input filesnames, 1st: Measured datafile. 2nd: sim datafile (by default factors.csv)", call.=FALSE)
}

nuclide<-expDataFile%>%strsplit(split = "\\.")%>%unlist()%>%nth(1)

simData<-read.csv(simDataFile, row.names = NULL)%>%gather( det, value, A0:B3)
expData<-read.csv(expDataFile, stringsAsFactors = FALSE)
names(expData)<-c("energy", "det", "value", "err")

toNA = function(i){
  if(length(i) == 0){
    return(NA)
  } else{
    return(i[1])
  }
}

data<-expData%>%rowwise()%>%mutate(simIndex = toNA(which(energy - 1 < simData$energy.keV. &
                                                      energy + 1 > simData$energy.keV. &
                                                      det == simData$det))
                                   )%>%
  na.omit()%>%mutate(simValue = simData$value[simIndex])%>%select(-simIndex)



gplot<-data%>%ggplot(aes(x=energy))+
  geom_point(aes(y=value, color="Measurement"),size=1)+
  geom_errorbar(mapping=aes(ymin=value-err, ymax=value+err), width=15, size=0.3,color="red")+
  geom_line(aes(y=simValue,color="Simulation"))+
  facet_grid(rows=vars(det))+
  scale_color_manual(values = c("red","black"),labels=c("Measurement","Simulation"))+
  labs(title=nuclide, x="Energy (keV)", y="summing correction factor")+
  theme(legend.title=element_blank(),legend.justification = "left", legend.position = "top")+
  guides(colour=guide_legend(override.aes=list(
    linetype=c (0, 1),shape=c(16,NA))))
ggsave(filename=paste0("./results/CompResult_",nuclide,".png",seq=''),plot=gplot,device =
         "png", path="./", height =7, width=5, units="in",dpi=500)


cat("combine.R has run successfully\n")

