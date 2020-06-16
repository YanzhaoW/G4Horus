transition<-"111Sn_111In"
nuclides<-unlist(strsplit(transition,"_"))
decays<-read.csv(paste(transition,".csv",sep=""), colClasses=c("character","numeric","character","character","numeric","numeric"))
index_ini0<-which(decays$InitialState == "0.00")
index_ini_non0<-which(decays$InitialState != "0.00")
decays[index_ini0,"InitialState"]<-paste(decays[index_ini0,"InitialState"],nuclides[1],sep="_")
decays[index_ini_non0,"InitialState"]<-paste(decays[index_ini_non0,"InitialState"],nuclides[2],sep="_")
decays[,"FinalState"]<-paste(decays[,"FinalState"],nuclides[2],sep="_")
write.csv(x=decays,file="../Decay_Scheme_Sn111.csv", row.names=FALSE, quote = FALSE)