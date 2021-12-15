//main.cpp  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "config.h" 


#define ADCCONFIGFILES  "../iio-hwmon.conf"   // ADCconf文件所在的文件位置
#define ADCMAXNUM  16                         // ADCconf文件中包含adc数量的最大值


enum sensor {
    ADC = 1,
    TEMPURATURE,
    AMPER,
};

void printHelp(void)
{
    printf("This software is modify conf-files\n");
    printf("  -h        show the help message!\n");
    printf("  -v        modify the adc warn-high/warn-low/critical-high/critical-low threshold;\n");
    printf("            params-1: sensor's name or the 'INTERVAL'\n");
    printf("            params-2: sensor's threshold-type, the params is {WH, WL, CH, CL}\n");
    printf("                      if the params-1 is INTERVAL, the params is the INTERVAL value(millisecond)\n");
    printf("            params-3: sensor's threshold' value, the value is multiply  1000\n");
    printf("                      if the params-1 is INTERVAL, the params-3 is NULL\n");
    
}

uint8_t setSensorInterval(char *str, uint8_t sensortype)
{
    char ConfigFile[100] = {'\0'};

    if (sensortype == ADC)
    {
        strcpy(ConfigFile, ADCCONFIGFILES);
    }
    else 
    {
        return -1;
    }

    Config configSettings(ConfigFile);
    
    configSettings.Add("INTERVAL", str);

    std::ofstream out(ConfigFile);
    out << configSettings;

    return 0;
}

uint8_t  setSensorProperty(char (*str)[30], uint8_t sensortype)
{
    char ConfigFile[100] = {'\0'};
    char tmp[50] = {'\0'};
    char threshType[10] = {'\0'};
    int i = 0;
    std::string label;
    
    if (sensortype == ADC)
    {
        strcpy(ConfigFile, ADCCONFIGFILES);
    }
    else
    {
        return -1;
    }
   
    Config configSettings(ConfigFile);
    for(i = 0; i <= ADCMAXNUM; i++)
    {
        sprintf(tmp, "LABEL_in%d", i);
        label = configSettings.Read(tmp, label);    
        if (strcmp(label.c_str(), str[0]) == 0)
            break;
    }
    if (i >  ADCMAXNUM)
    {
        printf("The sensor_name: %s is not matched!\n", str[0]);
        return -1;
    }

    if (strcmp(str[1], "WH") == 0)
    {
        strcpy(threshType, "WARNHI_in");
    }
    else if (strcmp(str[1], "WL") == 0)
    {
        strcpy(threshType, "WARNLO_in");
    }
    else if (strcmp(str[1], "CH") == 0)
    {
        strcpy(threshType, "CRITHI_in");
    }
    else if (strcmp(str[1], "CL") == 0)
    {
        strcpy(threshType, "CRITLO_in");
    }
    else
    {
        printf("The threshType:%s is wrong tyep!", str[1]);
        return -1;
    }

    sprintf(tmp, "%s%d", threshType, i);

    configSettings.Add(tmp, str[2]);
    
    std::ofstream out(ConfigFile);
    out << configSettings;

    return 0;
}

int main(int argc, char** argv)
{
    int c;
    char str[3][30];
    while (1)
    {
        c = getopt(argc, argv, "-ht:v:a:");
        if (c < 0)
            break;

        switch (c)
        {
            case 1:
                break;
            case 'h':
                printHelp();
                break;
            case 'v':                
                strcpy(str[0], optarg);
                strcpy(str[1], argv[optind]);

                if (strncmp(str[0], "INTERVAL", 4) != 0)
                {
                    strcpy(str[2], argv[optind + 1]);
                    setSensorProperty(str, ADC);
                }
                else 
                {
                    setSensorInterval(str[1], ADC);       
                }

                break;
        }
    }
}
