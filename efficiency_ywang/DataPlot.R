library(knitr)
library("stringr")
library(ggplot2)
library(dplyr)
library(magrittr)
library(ggpubr)

DetName=c("A0","A1","A2","A3","B0","B1","B2","B3")
plots <- vector(mode = "list", length = length(DetName))
sim_file = str_c("./Tables/", DetName,"_sim.csv",seq="")
ex_file = str_c("./efficiencies_10cm/", DetName,"_Ra.csv",seq="")
stat<-data.frame(matrix(ncol=2,nrow=8))
colnames(stat)<-c("t value", "p value")
rownames(stat)<-DetName

for(i in 1:length(DetName)){
  exDat<-read.csv(ex_file[i])
  simDat<-read.csv(sim_file[i])
  
  f<-approxfun(simDat$Energy,simDat$Efficiency)
  re<-t.test(exDat$Efficiency,f(exDat$Energy))
  stat[i,]= c(re$statistic,re$p.value)
  
  plot<-ggplot(exDat, aes(x=Energy, y=Efficiency))+
    geom_point(aes(color="black"))+
    scale_x_continuous(breaks = scales::pretty_breaks(n = 15)) +
    scale_y_continuous(breaks = scales::pretty_breaks(n = 8))+
    geom_errorbar(aes(ymin=Efficiency-Error, ymax=Efficiency+Error),
                  width=70) +
    geom_line(data = simDat, aes(colour = "red"))+
    labs(title = DetName[i], x="energy(keV)", y="efficiency")+
    scale_colour_manual(values=c("black","red"), 
                        labels = c("measurement","simulation") )+
    guides(colour=guide_legend(override.aes=list(
      linetype=c (0, 1),
      shape=c(16,NA)
    )
    ))+
    theme(plot.title = element_text(hjust = 0.5),
          legend.justification = c(1, 1), 
          legend.position = c(0.95, 0.95),
          legend.title=element_blank())+
    annotate(geom="label", x = 2250, y = 0.0035, 
             label = paste("t test: t=",round(re$statistic,2),seq=""),
             fill="white",label.size=0)
  plots[[i]] <- plot
  
}
cloA<-ggarrange(plots[[1]],plots[[2]],plots[[3]],plots[[4]], widths=c(1,1), heights=c(1,1))
cloB<-ggarrange(plots[[5]],plots[[6]],plots[[7]],plots[[8]], widths=c(1,1), heights=c(1,1))
ggsave(filename="CloA.png", plot=cloA, device="png",
       path="./", height=5, width=10, units="in", dpi=500)
ggsave(filename="CloB.png", plot=cloB, device="png",
       path="./", height=5, width=10, units="in", dpi=500)