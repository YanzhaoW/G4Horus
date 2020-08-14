getTable<-function(filename){
  data<-read.csv(file=filename, header=FALSE, sep=",", row.names = NULL,na.strings = c(""," ","NA"),stringsAsFactors=F)
  # data<-read.csv(file=filename, sep=",", row.names = NULL,na.strings = c(""," ","NA"),stringsAsFactors=F)
  names<-as.matrix(data[1,])[1,]
  names(data)<-sub(' ','_',trimws(names))
  data<-data[-1,]
  if(is.na(colnames(data)[ncol(data)])){
    data<-data[,1:length(data)-1]
  }else{
    data<-data[,1:length(data)]
  }
  paNu<-strsplit(filename,"_")[[1]][1]
  chNu<-strsplit(filename,"_")[[1]][2]
  data<-filter(data, symbol.1 == substr(chNu, nchar(chNu)-1,nchar(chNu)))
  is_gamma<-strsplit(strsplit(filename,"_")[[1]][3],"\\.")[[1]][1]=="1"
  
  if(is_gamma){
    data<-filter(data,is.na(`X-ray`))
    fdata<-data%>%select(energy, `intensity_%`,`daughter_level energy`,`end_level energy`,conversion_coeff.)
    names(fdata)<-c("energy","intensity","InitialState","FinalState","alpha")
    require(data.table)
    setDT(fdata)
    fdata<-na.omit(fdata, cols = c("intensity","InitialState","FinalState"))
    fdata$alpha=as.numeric(fdata$alpha)
    fdata$intensity=as.numeric(fdata$intensity)
    
    fdata$InitialState=as.numeric(fdata$InitialState)
    fdata$FinalState=as.numeric(fdata$FinalState)
    fdata$energy=as.numeric(fdata$energy)
    
    fdata$alpha[is.na(fdata$alpha)]<-0
    fdata$energy=format(round(fdata$energy,2),nsmall=2)
    fdata$InitialState=format(round(fdata$InitialState,2),nsmall=2)
    fdata$FinalState=format(round(fdata$FinalState,2),nsmall=2)
    
    fdata$energy=trimws(as.character(fdata$energy))
    fdata$intensity=trimws(as.character(fdata$intensity))
    fdata$InitialState=trimws(as.character(fdata$InitialState))
    fdata$FinalState=trimws(as.character(fdata$FinalState))
    fdata$FinalState<-paste0(fdata$FinalState,"_",chNu,seq='')
    fdata$InitialState<-paste0(fdata$InitialState,"_",chNu,seq='')
    fdata[,"gamma":=1]
  }else{
    fdata<-data.table(energy=c(0),intensity=data[[3]],InitialState=paste0("0.00_",paNu,seq=''),FinalState=
                        data[[5]], alpha=0,gamma=0)
    fdata<-na.omit(fdata, cols = c("intensity","InitialState"))
    fdata$FinalState=as.numeric(fdata$FinalState)
    fdata$FinalState=format(round(fdata$FinalState,2),nsmall=2)
    fdata$FinalState=trimws(as.character(fdata$FinalState))
    fdata$FinalState<-paste0(fdata$FinalState,"_",chNu,seq='')
  }
  return(fdata)
}
suppressMessages(library(plyr))
suppressMessages(library(dplyr))
suppressMessages(library(magrittr))
suppressMessages(library(ggpubr))
suppressMessages(library(data.table))
suppressMessages(library(stringr))

Tran152Eu<-c("152Eu","152Sm","152Gd","148Sm","144Nd","140Ce")
Tran226Ra<-c("226Ra","222Rn","218Po","214Pb","214Bi","214Po","210Pb","210Bi", "210Po","206Pb")

Tran<-Tran226Ra

files<-list.files(path="./", pattern = ".csv")
parfiles<-strsplit(files,"_")%>%lapply('[[',1)%>%unlist()
files_nu<-files[parfiles %in% Tran]
levels_l<-lapply(files_nu, getTable)
levels<-rbindlist(levels_l)
levels$energy<-as.numeric(levels$energy)
levels$energy=format(round(levels$energy,2),nsmall=2)
levels<-arrange(levels,energy)
levels$energy=trimws(as.character(levels$energy))
write.csv(x=levels, file =paste0("../../scripts/Decay_Scheme_",Tran[1],".csv",seq=''), quote=FALSE,row.names=FALSE)
cat(paste0("../../scripts/Decay_Scheme_",Tran[1],".csv",seq=''),"is created \n")


