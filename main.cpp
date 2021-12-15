//main.cpp  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "config.h" 



struct sensorInfo {
    char *type;
    char *sensorNameLabel;
    char *sensorWLlabel;
    char *sensorWHlabel;
    char *sensorCLlabel;
    char *sensorCHlabel;
    char *sensorPath;
    int sensorMaxNum;
};

struct sensorInfo tsensorInfo[] = {
    {"ADC", "LABEL_in", "WARNLO_in", "WARNHI_in", "CRITLO_in", "CRITHI_in", "../iio-hwmon.conf", 1},
    {"TEMPURATURE", "LABEL_temp", "WARNLO_temp", "WARNHI_temp", "CRITLO_temp", "CRITHI_temp", "../tmp421@1e.conf", 1},
    {"TEMPURATURE", "LABEL_temp", "WARNLO_temp", "WARNHI_temp", "CRITLO_temp", "CRITHI_temp", "../tmp421@2a.conf", 1},
    {"TEMPURATURE", "LABEL_temp", "WARNLO_temp", "WARNHI_temp", "CRITLO_temp", "CRITHI_temp", "../tmp421@4e.conf", 1}
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
    printf("  -q        modify the curr, tempature sensor threshlod, the params is same as adc\n");
    printf("  -t        modify the interval time\n");
    
}

void addquota(char *str)
{
    char tmp[50] = {'\0'};
    int i = 0;

    tmp[i] = '"';
    for (i = 0; str[i] != '\0'; i++)
    {
        tmp[i + 1] = str[i];
    }

    tmp[i + 1] = '"';
    tmp[i + 2] = '\0';

    strcpy(str, tmp);
}

uint8_t setSensorInterval(char *str)
{
    int i = 0;
    unsigned int j = 0;
    struct sensorInfo *ptsensorInfo = NULL;

    for (j = 0; j < sizeof(tsensorInfo)/sizeof(sensorInfo); j++)
    {
        ptsensorInfo = &tsensorInfo[j];

        Config configSettings(ptsensorInfo->sensorPath);
        
        configSettings.Add("INTERVAL", str);

        std::ofstream out(ptsensorInfo->sensorPath);

        out << configSettings;
    }
    
    return 0;
}

uint8_t  setSensorProperty(char (*str)[30])
{
    char tmp[50] = {'\0'};
    char threshType[30] = {'\0'};
    int i = 0;
    unsigned int j = 0;
    struct sensorInfo *ptsensorInfo = NULL;
    std::string label;

    for (j = 0; j < sizeof(tsensorInfo)/sizeof(sensorInfo); j++)
    {
        ptsensorInfo = &tsensorInfo[j];

        Config configSettings(ptsensorInfo->sensorPath);

        for(i = 0; i <= ptsensorInfo->sensorMaxNum; i++)
        {
            sprintf(tmp, "%s%d", ptsensorInfo->sensorNameLabel, i);
            label = configSettings.Read(tmp, label);    
            if (strcmp(label.c_str(), str[0]) == 0)
                break;
        }

        if (i <= ptsensorInfo->sensorMaxNum)
            break;
    }

    if (j == sizeof(tsensorInfo)/sizeof(sensorInfo))
    {
        printf("cannot find the property sensortype\n");
        return -1;
    }

    Config configSettings(ptsensorInfo->sensorPath);    

    if (strcmp(str[1], "WH") == 0)
    {
        strcpy(threshType, ptsensorInfo->sensorWHlabel);
    }
    else if (strcmp(str[1], "WL") == 0)
    {
        strcpy(threshType, ptsensorInfo->sensorWLlabel);
    }
    else if (strcmp(str[1], "CH") == 0)
    {
        strcpy(threshType, ptsensorInfo->sensorCHlabel);
    }
    else if (strcmp(str[1], "CL") == 0)
    {
        strcpy(threshType, ptsensorInfo->sensorCLlabel);
    }
    else
    {
        printf("The threshType:%s is wrong tyPE!", str[1]);
        return -1;
    }

    sprintf(tmp, "%s%d", threshType, i);
    configSettings.Add(tmp, str[2]);
    
    std::ofstream out(ptsensorInfo->sensorPath);

    out << configSettings;

    return 0;
}

int main(int argc, char** argv)
{
    int c;
    char str[3][30];

    while (1)
    {
        c = getopt(argc, argv, "-hv:q:t:");
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

                strcpy(str[2], argv[optind + 1]);
                setSensorProperty(str);

                break;

            case 'q':
                strcpy(str[0], optarg);
                strcpy(str[1], argv[optind]);

                strcpy(str[2], argv[optind + 1]);
                addquota(str[0]);
                addquota(str[2]);
                setSensorProperty(str);

                break;
            case 't':
                setSensorInterval(optarg);
                break;

            default:
                printHelp();
                break;
        }
    }

    exit(0);
}
