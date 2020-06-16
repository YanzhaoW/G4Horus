library(tidyr)
library(dplyr)
library(data.table)
library(ggplot2)
nuclide<-"152Eu"
data<-read.csv("SummingCorrectionFactor.csv",row.names = NULL)
data<-filter(data,Nuclide==nuclide)
simData<-read.csv("factors.csv", row.names = NULL)
simData<-simData[,1:length(simData)-1]
names(simData)[1] <- "Energy"
simData<-pivot_longer(simData,cols= c("A0","A1","A2","A3","B0","B1","B2","B3"),names_to=
                       "Detector", values_to = "sim_z")
require(data.table)
setDT(simData)
setDT(data)
simData<-arrange(simData, Energy, Detector)
data<-arrange(data,Energy,Detector)
simData$Energy<-data$Energy
cor_full<-merge(data,simData,by=c("Energy","Detector"))
cor_full<-arrange(cor_full, Energy, Detector)
write.csv(x=cor_full, file =paste0("Summing_Sim_Ex_Comp_",nuclide,".csv",seq=''), quote=FALSE, na="NA")

gplot<-cor_full%>%drop_na()%>%ggplot(aes(x=Energy))+
  geom_point(aes(y=z, color="Measurement"),size=1)+
  geom_errorbar(mapping=aes(ymin=z-dz, ymax=z+dz), width=15, size=0.3,color="red")+
  geom_line(aes(y=sim_z,color="Simulation"))+
  facet_grid(rows=vars(Detector))+
  scale_color_manual(values = c("red","black"),labels=c("Measurement","Simulation"))+
  labs(title=nuclide, x="Energy (keV)", y="summing correction factor")+
  theme(legend.title=element_blank(),legend.justification = "left", legend.position = "top")+
  guides(colour=guide_legend(override.aes=list(
    linetype=c (0, 1),shape=c(16,NA))))
gplot
ggsave(filename=paste0("Summing_Sim_Ex_Comp_",nuclide,".png",seq=''),plot=gplot,device =
         "png", path="./", height =7, width=5, units="in",dpi=500)
