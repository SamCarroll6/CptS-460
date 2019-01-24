
int strlen(char *str1)
{
    int count = 0;
    while(*str1)
    {
        count++;
        str1++;
    }
    return count;
}

int strcmp(char *str1, char *str2)
{
    if(strlen(str1) != strlen(str2))
    {
        return 1;
    }
    while(*str1 && *str2)
    {
        if(*str1 != *str2)
        {
            return 1;
        }
        str1++;
        str2++;
    }
    return 0;
}