//
//  EETCC.c
//  Echonet Lite Master Node
//
//  Created by QuadCube on 11/07/2016.
//  Copyright © 2016 QuadCube. All rights reserved.
//

#include "EETCC.h"

double PMV,PPD;
float firstTimeValue,prev_firstTimeValue;
uint8_t globalAdelay_flag,prev_controlSignal_flag;
uint8_t prev_controlSignal=5,controlSignal;
time_t currentTime,previousTime;
double secondTime;

/*
 EETCC Initialization
 Note: 1) Initialize Global A value, globalAdelay_flag,prev & first_time_value,previous_time
 */
void EETCC_init(void)
{
    prev_firstTimeValue=firstTimeValue=0.4;
    globalAdelay_flag=0;
    prev_controlSignal_flag=0;
    time(&previousTime);
}

/*
 EETCC Thermal Comfort => Fanger Thermal Comfort Equation
 Note: 1) Predicted Mean Vote (PMV) & Predicted Percentage Dissatisfied (PPD)
 */
void EETCC_thermalComfort(float clothing_insulation,float metabolic_rate,float velocity,float temperature_mean_radiation,float relative_humidity,float air_temperature)
{
    double WME=0;    //assumption that no external work is done
    double FNPS,PA,ICL,M,W,MW,FCL,HFC,TAA,TRA,TCLA,P1,P2,P3,P4,P5,XN,XF,EPS,HCN,HC,TCL,HL1,HL2,HL3,HL4,HL5,HL6,TS;
    uint8_t N=0;    //number of iterations
    uint8_t flag=0; //iteration flag
    FNPS=exp(16.6536-(4030.183/(air_temperature+235))); //saturated vapour pressure (kPa)
    PA=relative_humidity*10*FNPS;                       //water vapour pressure (Pa)
    ICL=0.155*clothing_insulation;                      //thermal insulation of the clothing (m^2K/W)
    M=metabolic_rate*58.15;                             //metabolic rate (W/m^2)
    W=WME*58.15;                                        //external work (W/m^2)
    MW=M-W;                                             //internal heat production in human body (W/m^2)
    if(ICL<0.078)
        FCL=1+(1.29*ICL);                               //clothing area factor
    else
        FCL=1.05+(0.645*ICL);
    if(velocity<=0)
        HFC=0;                                          //heat transfer coefficient by forced convection
    else
        HFC=12.1*sqrt(velocity);
    TAA=air_temperature+273;                            //air temperature (K)
    TRA=temperature_mean_radiation+273;                 //mean radiant temperature (K)
    
    //calculation for surface temperature of clothing by iteration
    TCLA=TAA+((35.5-air_temperature)/(3.5*((6.45*ICL)+0.1)));
    //first guess for surface temperature of clothing
    P1=ICL*FCL;                                         //calculation term
    P2=P1*3.96;                                         //calculation term
    P3=P1*100;                                          //calculation term
    P4=P1*TAA;                                          //calculation term
    P5=308.7-(0.028*MW)+(P2*pow((TRA/100),4));
    XN=TCLA/100;
    XF=XN;
    EPS=0.00015;                                        //stop criteria in iteration
    while((N<=150) && (flag==0))
    {
        XF=(XF+XN)/2;
        HCN=2.38*pow(fabs((100*XF)-TAA),0.25);          //heat transfer coefficient by natural convection note: check fabs
        if(HFC>HCN)
            HC=HFC;
        else
            HC=HCN;
        XN=(P5+P4*HC-P2*pow(XF,4))/(100+P3*HC);
        N++;
        if(fabs(XF-XN)<EPS)
            flag=1;
        else
            flag=0;
    }
    HCN=2.38*pow(fabs((100*XF)-TAA),0.25);
    if(HFC>HCN)
        HC=HFC;
    else
        HC=HCN;
    TCL=100*XN-273;                                     //surface temperature of the clothing
    
    //calculation for heat loss components
    HL1=3.05*0.001*(5733-6.99*MW-PA);                   //heat loss diffusion through skin
    if(MW>58.15)
        HL2=0.42*(MW-58.15);                            //heat loss by sweating (comfort)
    else
        HL2=0;
    HL3=1.7*0.00001*M*(5867-PA);                        //latent respiration heat loss
    HL4=0.0014*M*(34-air_temperature);                  //dry respiration heat loss
    HL5=3.96*FCL*(pow(XN,4)-pow((TRA/100),4));          //heat loss by radiation
    HL6=FCL*HC*(TCL-air_temperature);                   //heat loss by convection
    
    //calculation for PMV and PPD
    TS=0.303*exp(-0.036*M)+0.028;                       //thermal sensation transfer coefficient
    PMV=TS*(MW-HL1-HL2-HL3-HL4-HL5-HL6);                //predicted mean vote
    PPD=100-95*exp(-0.03353*pow(PMV,4)-0.2179*pow(PMV,2));   //predicted percentage dissatisfied
    //printf("PMV: %f PPD: %f\n",PMV,PPD);
}

/*
 EETCC PMV
 Note: 1) Return PMV value
 */
double EETCC_PMV(void)
{
    return PMV;
}

/*
 EETCC PPD
 Note: 1) Return PPD value
 */
double EETCC_PPD(void)
{
    return PPD;
}

/*
 EETCC Previous Control
 Note: 1) Modified from the original previous_control due to the way the function is used in the main program
 */
uint8_t EETCC_prev_controlSignal(void)
{
    //uint8_t memory[2]={5,5};
    //memory[0]=memory[1];
    //memory[1]=controlSignal;
    
    if(prev_controlSignal_flag==1)
    {
        //prev_controlSignal=controlSignal;
        prev_controlSignal_flag=0;
        //return prev_controlSignal;
    }
    if(prev_controlSignal!=controlSignal)
    {
        prev_controlSignal=controlSignal;
        prev_controlSignal_flag=1;
    }
    //return prev_controlSignal;
    //return memory[0];
    return controlSignal;
}

/*
 EETCC Draught
 Note: 1) Draught Rate calculation for EETCC_controlSignal
 */
double EETCC_draught(float local_air_temperature,float local_mean_air_velocity)
{
    if(local_mean_air_velocity>=0.5)
        return 100;
    else if(local_mean_air_velocity<=0.05)
        return 0;
    else
        return ((34-local_air_temperature)*(pow((local_mean_air_velocity-0.05),0.62))*(0.37*local_mean_air_velocity*0.4+3.14));
}

/*
 EETCC Global A
 Note: 1) Used to initiatize the starting condition of EETCC algorithm
 */
float EETCC_globalA(double PMV)
{
    if(PMV>=firstTimeValue && PMV<=0.5)
        firstTimeValue=0;
    return firstTimeValue;
}

/*
 EETCC Global A
 Note: 1) Dependency (EETCC_globalA)
       2) 1 step delay of globalA
 */
float EETCC_globalA_delay(void)
{
    if(globalAdelay_flag==1)
    {
        prev_firstTimeValue=firstTimeValue;
        globalAdelay_flag=0;
        return prev_firstTimeValue;
    }
    if(prev_firstTimeValue!=firstTimeValue)
        globalAdelay_flag=1;
    return prev_firstTimeValue;
}

/*
 EETCC Q1
 Note: 1) Require further validation,mismatch from Simulink block around (value+- 45.8)
 */
float EETCC_Q1(float occupant,float local_air_temperature,float outdoor_air_temperature,float correction,float solar_south,float solar_west)
{
    return (((1.815+1.815)*2.2045+0.66*1.3374)*(outdoor_air_temperature-local_air_temperature)*(correction))+(((230*1+190)/3.412142)*(occupant))+((((1.815+1.815)*0.44029579)*(solar_south))+((0.66*0.31533579)*(solar_west)));
}

/*
 EETCC Q2
 Note: 1) Require further validation,mismatch from Simulink block around (value+- 45.8)
 */
float EETCC_Q2(float occupant,float local_air_temperature,float outdoor_air_temperature,float correction,float solar_south,float solar_west)
{
    return (((outdoor_air_temperature-local_air_temperature)*((1.815+1.815)*2.2045+0.66*1.3374)*correction)+(occupant*((230*1+190)/3.412142))+((solar_south*((1.815+1.815)*0.79))+(solar_west*(0.66*0.41))));
}

/*
 EETCC Timer
 Note: 1) Timer to regulate on/off of air conditioner to prevent to frequent switching
       2) Uses current and previous control signal to monitor
 */
float EETCC_timer(void)
{
    if(prev_controlSignal_flag==1)
    {
        time(&previousTime);
        return 0;
    }
    time(&currentTime);
    return ((float)difftime(currentTime,previousTime)/10);
}


/*
 EETCC Control Signal (Original: v1)
 Note: 1) Dependency (EETCC_thermalComfort,EETCC_index,EETCC_GlobalA,EETCC_GlobalA_delay)
       2) Algorithm is executed every 10 second (Total: 8640 data point for 24 Hours)
 */
uint8_t EETCC_controlSignal_v1(double PMV1,double PMV2,double PMV3,double PMV4,uint8_t prev_EETCC_controlSignal,float timer,uint8_t index,float local_mean_air_velocity,float temperature_mean_radiation,float GlobalA,float GlobalA_delay)
{
    uint8_t i,j,iAll=1,iPart=1,iArray=1,counter_arrayAll=0,counter_newArray=0,control;
    uint8_t indicate[4],PMV_index[4];
    float timesh;
    double mAll,mArray;
    double PMVoffset=PMV_OFFSET,DRThreshold=DRAUGHT_THRESHOLD,thresholdValue_PMV=PMV_THRESHOLD;
    double arrayAll[4]={0.0,0.0,0.0,0.0},All[4]={PMV1,PMV2,PMV3,PMV4},arrayPart[2]={fabs(PMV1),fabs(PMV2)},newArray[4],array[4];
    
    if(prev_EETCC_controlSignal==1 || prev_EETCC_controlSignal==2 || prev_EETCC_controlSignal==3 || prev_EETCC_controlSignal==4)
        timesh=(MINIMUM_TIME_WITHOUT_USING_AIRCOND*6);
    else
        timesh=(MINIMUM_TIME_USING_AIRCOND*6);
    //Draught demands
    if((EETCC_draught(temperature_mean_radiation,(0.8*local_mean_air_velocity)))>DRThreshold)
        counter_arrayAll=2;
    else if(((EETCC_draught(temperature_mean_radiation,(0.8*local_mean_air_velocity)))<=DRThreshold) && ((EETCC_draught(temperature_mean_radiation,local_mean_air_velocity))>DRThreshold))
        counter_arrayAll=3;
    else
        counter_arrayAll=4;
    arrayAll[0]=fabs(PMV1);
    arrayAll[1]=fabs(PMV2);
    if(counter_arrayAll==3 || counter_arrayAll==4)
    {
        arrayAll[2]=fabs(PMV3);
        if(counter_arrayAll==4)
            arrayAll[3]=fabs(PMV4);
    }
    mAll=arrayAll[0];
    for(i=1;i<(counter_arrayAll);i++) //min(arrayAll_abs)
        if(arrayAll[i]<mAll)
        {
            mAll=arrayAll[i];
            iAll=(i+1); //keeping iAll in MATLAB format (iAll must be a non zero)
        }
    if(arrayPart[0]<=arrayPart[1])  //min(arrayPart_abs)
        iPart=1;
    else
        iPart=2;
    
    if(GlobalA!=0)
        control=5;   //return control: 5
    else
    {
        if(timer>timesh)
        {
            if(mAll<PMVoffset)
            {
                if(prev_EETCC_controlSignal==1 || prev_EETCC_controlSignal==2 || prev_EETCC_controlSignal==3 || prev_EETCC_controlSignal==4)
                {
                    if((fabs(mAll-All[prev_EETCC_controlSignal-1]))<thresholdValue_PMV)
                        control=prev_EETCC_controlSignal;    //return control: previous
                    else
                    {
                        for(i=0;i<counter_arrayAll;i++) //indicate=arrayAll<PMVoffset
                        {
                            if(arrayAll[i]<PMVoffset)
                            {
                                indicate[i]=1;
                                newArray[counter_newArray]=arrayAll[i]; //indicate=arrayAll_abs(indicate)
                                counter_newArray++;
                            }
                            else
                                indicate[i]=0;
                        }
                        mArray=array[0]=fabs(newArray[0]-All[prev_EETCC_controlSignal-1]);
                        for(i=1;i<counter_newArray;i++) //[~,i]=min(array)
                        {
                            array[i]=fabs(newArray[i]-All[prev_EETCC_controlSignal-1]);
                            if(array[i]<mArray)
                            {
                                mArray=array[i];
                                iArray=i+1;
                            }
                        }
                        j=0;
                        for(i=0;i<counter_arrayAll;i++) //PMV_index=find(indicate==1)
                        {
                            if(indicate[i]==1)
                            {
                                PMV_index[j]=i+1;   //j cannot exceed counter_newArray
                                j++;
                            }
                        }
                        control=PMV_index[iArray-1];
                        if(control==1 && index==2)
                            control=2;
                    }
                }
                else
                {
                    if(GlobalA_delay==0)
                        control=iAll;
                    else
                    {
                        for(i=0;i<counter_arrayAll;i++) //indicate=arrayAll<PMVoffset
                        {
                            if(arrayAll[i]<PMVoffset)
                            {
                                indicate[i]=1;
                                newArray[counter_newArray]=arrayAll[i]; //indicate=arrayAll_abs(indicate)
                                counter_newArray++;
                            }
                            else
                                indicate[i]=0;
                        }
                        mArray=array[0]=fabs(newArray[0]-(0.8*PMVoffset));
                        for(i=1;i<counter_newArray;i++) //[~,i]=min(array)
                        {
                            array[i]=fabs(newArray[i]-(0.8*PMVoffset));
                            if(array[i]<mArray)
                            {
                                mArray=array[i];
                                iArray=i+1;
                            }
                        }
                        j=0;
                        for(i=0;i<counter_arrayAll;i++) //PMV_index=find(indicate==1)
                        {
                            if(indicate[i]==1)
                            {
                                PMV_index[j]=i+1;   //j cannot exceed counter_newArray
                                j++;
                            }
                        }
                        control=PMV_index[iArray-1];
                    }
                    if(control==1 && index==2)
                        control=2;
                }
            }
            else
            {
                if(prev_EETCC_controlSignal==1 || prev_EETCC_controlSignal==2) //modification from original (control_old==1 && control_old==2),which is impossible for control_old to hold 2 state at the same time
                    control=prev_EETCC_controlSignal+4;
                else
                    control=iPart+4;
                if(control==5 && index==2)
                    control=6;
            }
        }
        else
            control=prev_EETCC_controlSignal;
    }
    controlSignal=control;
    return control;
}

/*
 EETCC Control Signal (Modified: v2)
 Note: 1) Dependency (EETCC_thermalComfort,EETCC_index,EETCC_GlobalA,EETCC_GlobalA_delay)
       2) Algorithm is executed every 10 second (Total: 8640 data point for 24 Hours)
 */
uint8_t EETCC_controlSignal_v2(double PMV1,double PMV2,double PMV3,double PMV4,uint8_t prev_EETCC_controlSignal,float timer,uint8_t index,float local_mean_air_velocity,float temperature_mean_radiation,float temperature_outdoor,float solar_radiation,float GlobalA,float GlobalA_delay)
{
    uint8_t i,j,iAll=1,iPart=1,iArray=1,counter_arrayAll=0,counter_newArray=0,control;
    uint8_t indicate[4],PMV_index[4];
    float timesh;
    double mAll,mArray;
    double PMVoffset=PMV_OFFSET,DRThreshold=DRAUGHT_THRESHOLD,thresholdValue_PMV=PMV_THRESHOLD;
    double arrayAll[4]={0.0,0.0,0.0,0.0},All[4]={PMV1,PMV2,PMV3,PMV4},arrayPart[2]={fabs(PMV1),fabs(PMV2)},newArray[4],array[4];
    
    //modified EETCC    12/8/2016
    if(prev_EETCC_controlSignal==1 || prev_EETCC_controlSignal==2 || prev_EETCC_controlSignal==3 || prev_EETCC_controlSignal==4)
    {
        if(temperature_outdoor>=temperature_mean_radiation)
            timesh=(MINIMUM_TIME_WITHOUT_USING_AIRCOND_ToutHi*6);
        else
            timesh=(MINIMUM_TIME_WITHOUT_USING_AIRCOND_ToutLOW*6);
    }
    else
    {
        if(temperature_outdoor>=temperature_mean_radiation)
            timesh=(MINIMUM_TIME_USING_AIRCOND_ToutHI*6);
        else
            timesh=(MINIMUM_TIME_USING_AIRCOND_ToutLOW*6);
    }
    //end of modified EETCC 12/8/2016
    //Draught demands
    if((EETCC_draught(temperature_mean_radiation,(0.8*local_mean_air_velocity)))>DRThreshold)
        counter_arrayAll=2;
    else if(((EETCC_draught(temperature_mean_radiation,(0.8*local_mean_air_velocity)))<=DRThreshold) && ((EETCC_draught(temperature_mean_radiation,local_mean_air_velocity))>DRThreshold))
        counter_arrayAll=3;
    else
        counter_arrayAll=4;
    arrayAll[0]=fabs(PMV1);
    arrayAll[1]=fabs(PMV2);
    if(counter_arrayAll==3 || counter_arrayAll==4)
    {
        arrayAll[2]=fabs(PMV3);
        if(counter_arrayAll==4)
            arrayAll[3]=fabs(PMV4);
    }
    mAll=arrayAll[0];
    for(i=1;i<(counter_arrayAll);i++) //min(arrayAll_abs)
        if(arrayAll[i]<mAll)
        {
            mAll=arrayAll[i];
            iAll=(i+1); //keeping iAll in MATLAB format (iAll must be a non zero)
        }
    if(arrayPart[0]<=arrayPart[1])  //min(arrayPart_abs)
        iPart=1;
    else
        iPart=2;
    
    if(GlobalA!=0)
        control=5;   //return control: 5
    else
    {
        if(timer>timesh)
        {
            if(mAll<PMVoffset)
            {
                if(prev_EETCC_controlSignal==1 || prev_EETCC_controlSignal==2 || prev_EETCC_controlSignal==3 || prev_EETCC_controlSignal==4)
                {
                    if((fabs(mAll-All[prev_EETCC_controlSignal-1]))<thresholdValue_PMV)
                        control=prev_EETCC_controlSignal;    //return control: previous
                    else
                    {
                        for(i=0;i<counter_arrayAll;i++) //indicate=arrayAll<PMVoffset
                        {
                            if(arrayAll[i]<PMVoffset)
                            {
                                indicate[i]=1;
                                newArray[counter_newArray]=arrayAll[i]; //indicate=arrayAll_abs(indicate)
                                counter_newArray++;
                            }
                            else
                                indicate[i]=0;
                        }
                        mArray=array[0]=fabs(newArray[0]-All[prev_EETCC_controlSignal-1]);
                        for(i=1;i<counter_newArray;i++) //[~,i]=min(array)
                        {
                            array[i]=fabs(newArray[i]-All[prev_EETCC_controlSignal-1]);
                            if(array[i]<mArray)
                            {
                                mArray=array[i];
                                iArray=i+1;
                            }
                        }
                        j=0;
                        for(i=0;i<counter_arrayAll;i++) //PMV_index=find(indicate==1)
                        {
                            if(indicate[i]==1)
                            {
                                PMV_index[j]=i+1;   //j cannot exceed counter_newArray
                                j++;
                            }
                        }
                        control=PMV_index[iArray-1];
                        if(control==1 && index==2)
                            control=2;
                    }
                }
                else
                {
                    if(GlobalA_delay==0)
                        control=iAll;
                    else
                    {
                        for(i=0;i<counter_arrayAll;i++) //indicate=arrayAll<PMVoffset
                        {
                            if(arrayAll[i]<PMVoffset)
                            {
                                indicate[i]=1;
                                newArray[counter_newArray]=arrayAll[i]; //indicate=arrayAll_abs(indicate)
                                counter_newArray++;
                            }
                            else
                                indicate[i]=0;
                        }
                        mArray=array[0]=fabs(newArray[0]-(0.8*PMVoffset));
                        for(i=1;i<counter_newArray;i++) //[~,i]=min(array)
                        {
                            array[i]=fabs(newArray[i]-(0.8*PMVoffset));
                            if(array[i]<mArray)
                            {
                                mArray=array[i];
                                iArray=i+1;
                            }
                        }
                        j=0;
                        for(i=0;i<counter_arrayAll;i++) //PMV_index=find(indicate==1)
                        {
                            if(indicate[i]==1)
                            {
                                PMV_index[j]=i+1;   //j cannot exceed counter_newArray
                                j++;
                            }
                        }
                        control=PMV_index[iArray-1];
                    }
                    if(control==1 && index==2)
                        control=2;
                }
            }
            else
            {
                if(prev_EETCC_controlSignal==1 || prev_EETCC_controlSignal==2) //modification from original (control_old==1 && control_old==2),which is impossible for control_old to hold 2 state at the same time
                    control=prev_EETCC_controlSignal+4;
                else
                    control=iPart+4;
                if(control==5 && index==2)
                    control=6;
            }
            //modified EETCC    12/8/2016
            if(PMV1<=-0.5)
            {
                if(control==5)
                    control=1;
                else if(control==6)
                    control=2;
            }
            if((control==3 || control==4) && (temperature_outdoor>=temperature_mean_radiation) && (solar_radiation>=1.0))
                control=1;
            if((control==5 || control==6) && (temperature_outdoor<=temperature_mean_radiation) && (solar_radiation<=1.0))
                control=4;
            if(control==6 && solar_radiation>=1.0)
                control=5;
            else if(control==5 && solar_radiation<=1.0)
                control=6;
            //end of modified EETCC 12/8/2016
        }
        else
            control=prev_EETCC_controlSignal;
    }
    controlSignal=control;
    return control;
}

/*
 EETCC Index Signal
 Note: 1) Compare Q1 and Q2, then passed to EETCC_controlSignal
 */
uint8_t EETCC_index(float temperature_mean_radiation,float desired_temperature,float Q1,float Q2)
{
    if(temperature_mean_radiation>desired_temperature)
    {
        if(Q1<=Q2)
            return 1;
        else
            return 2;
    }
    else
    {
        if(Q1>=Q2)
            return 1;
        else
            return 2;
    }
}

/*
 EETCC
 Note:
 */

