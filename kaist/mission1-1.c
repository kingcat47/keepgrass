#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int data;
    int time;
    int important;
    int group;
    char title[128];
    char mosiggangi[128];
} Work;

typedef struct
{
    Work **work;
    int count;
    int nowmax;
} Arr;

const char *GROUP_NAMES[] = {
    "Study", "Counsel", "Meeting", "Break", "Other"};

void make(Arr *arr)
{
    arr->work = (Work **)malloc(8 * sizeof(Work *));
    arr->count = 0;
    arr->nowmax = 8;
}

void plus(Arr *arr, Work *work)
{
    if (arr->count >= arr->nowmax)
    {
        arr->nowmax *= 2;
        arr->work = (Work **)realloc(arr->work, arr->nowmax * sizeof(Work *));
    }
    arr->work[arr->count++] = work;
}

void format_date(int date, char *formatted)
{
    sprintf(formatted, "%d.%02d.%02d.",
            date / 10000,
            (date % 10000) / 100,
            date % 100);
}

void format_time(int time, char *formatted)
{
    sprintf(formatted, "%02d:%02d",
            time / 100,
            time % 100);
}

void print_work(Arr *groups)
{
    printf("\n%-14s %-8s %-8s %-8s %-16s %s\n",
           "Date", "Time", "Priority", "Group", "Title", "Description");
    printf("----------------------------------------------------------------\n");

    for (int g = 0; g < 5; g++)
    {
        for (int i = 0; i < groups[g].count; i++)
        {
            Work *work = groups[g].work[i];
            char formatted_date[15];
            char formatted_time[6];

            format_date(work->data, formatted_date);
            format_time(work->time, formatted_time);

            printf("%-14s %-8s %-8d %-8s %-16s %s\n",
                   formatted_date,
                   formatted_time,
                   work->important,
                   GROUP_NAMES[work->group],
                   work->title,
                   work->mosiggangi);
        }
    }
}

void input_schedule(FILE *fp)
{
    printf("Enter schedule (Date Time Priority Group Title Description)\n");
    printf("Example: 20170302 0900 8 0 ClassTime CProgramming\n");
    printf("Press Ctrl+Z(Windows)\n\n");

    Work input;
    while (scanf("%d %d %d %d %s %s",
                 &input.data, &input.time, &input.important, &input.group,
                 input.title, input.mosiggangi) == 6)
    {
        fprintf(fp, "%d %d %d %d %s %s\n",
                input.data, input.time, input.important, input.group,
                input.title, input.mosiggangi);
    }

    fclose(fp);
}

int main()
{
    FILE *write_fp = fopen("data.txt", "w");
    if (!write_fp)
    {
        printf("Cannot open file.\n");
        return 1;
    }

    input_schedule(write_fp);

    FILE *read_fp = fopen("data.txt", "r");
    if (!read_fp)
    {
        printf("Cannot open file.\n");
        return 1;
    }

    Arr arr[5];
    for (int i = 0; i < 5; i++)
    {
        make(&arr[i]);
    }

    while (1)
    {
        Work *work = (Work *)malloc(sizeof(Work));
        if (fscanf(read_fp, "%d %d %d %d %s %s",
                   &work->data,
                   &work->time,
                   &work->important,
                   &work->group,
                   work->title,
                   work->mosiggangi) != 6)
        {
            free(work);
            break;
        }
        if (work->group < 5)
        {
            plus(&arr[work->group], work);
        }
    }

    print_work(arr);

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < arr[i].count; j++)
        {
            free(arr[i].work[j]);
        }
        free(arr[i].work);
    }

    fclose(read_fp);
    return 0;
}