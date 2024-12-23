#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define SPACE 32 // Spacebar key
#define UP 72    // Arrow key: Up
#define DOWN 80  // Arrow key: Down

// Work structure to represent a schedule item
typedef struct
{
    int data;              // Date (YYYYMMDD format)
    int time;              // Time (HHMM format)
    int important;         // Priority (0~10)
    int group;             // Group ID (0~4)
    char title[128];       // Work title
    char description[128]; // Work description
} Work;

// Array structure to manage work items
typedef struct
{
    Work **work;     // Array of work pointers
    int count;       // Number of current items
    int nowmax;      // Maximum capacity of the array
    int total_items; // Total number of items
} Arr;

const char *GROUP_NAMES[] = {"Study", "Counsel", "Meeting", "Break", "Other"};

// Initialize the array
void make(Arr *arr)
{
    arr->work = (Work **)malloc(8 * sizeof(Work *));
    arr->count = 0;
    arr->nowmax = 8;
    arr->total_items = 0;
}

// Add a work item to the array
void plus(Arr *arr, Work *work)
{
    if (arr->count >= arr->nowmax)
    {
        arr->nowmax *= 2;
        arr->work = (Work **)realloc(arr->work, arr->nowmax * sizeof(Work *));
    }
    arr->work[arr->count++] = work;
}

// Format the date
void format_date(int date, char *formatted)
{
    sprintf(formatted, "%d.%02d.%02d.", date / 10000, (date % 10000) / 100, date % 100);
}

// Format the time
void format_time(int time, char *formatted)
{
    sprintf(formatted, "%02d:%02d", time / 100, time % 100);
}

// Clear the screen
void clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Print a single work item
void print_item(Work *work, int is_selected)
{
    char formatted_date[15];
    char formatted_time[6];

    format_date(work->data, formatted_date);
    format_time(work->time, formatted_time);

    printf("%s%-14s %-8s %-8d %-8s %-16s\n",
           is_selected ? "\u25BA " : "  ",
           formatted_date, formatted_time, work->important,
           GROUP_NAMES[work->group], work->title);
}

// Print the menu
void print_menu(Arr *groups, int selected)
{
    clear_screen();
    printf("\n%-14s %-8s %-8s %-8s %-16s\n",
           "Date", "Time", "Priority", "Group", "Title");
    printf("---------------------------------------------------------------\n");

    int current_item = 0;
    for (int g = 0; g < 5; g++)
    {
        for (int i = 0; i < groups[g].count; i++)
        {
            print_item(groups[g].work[i], current_item == selected);
            current_item++;
        }
    }
}

// Get the total number of items
int get_total_items(Arr *groups)
{
    int total = 0;
    for (int i = 0; i < 5; i++)
    {
        total += groups[i].count;
    }
    return total;
}

// Input schedules interactively
void input_schedule(FILE *fp, Arr *groups)
{
    printf("Enter schedules in the format: YYYYMMDD HHMM PRIORITY GROUP TITLE DESCRIPTION\n");
    printf("Example: 20231222 1300 5 0 Meeting ImportantMeeting\n");
    printf("Press Ctrl+Z (Windows) or Ctrl+D (Unix) to finish input.\n\n");

    Work *input = (Work *)malloc(sizeof(Work));
    while (scanf("%d %d %d %d %s %s", &input->data, &input->time, &input->important,
                 &input->group, input->title, input->description) == 6)
    {
        if (input->group >= 0 && input->group < 5)
        {
            plus(&groups[input->group], input);
            fprintf(fp, "%d %d %d %d %s %s\n", input->data, input->time, input->important,
                    input->group, input->title, input->description);
            input = (Work *)malloc(sizeof(Work));
        }
        else
        {
            printf("Invalid group ID. Please try again.\n");
        }
    }
    free(input);
}

int main()
{
    FILE *write_fp = fopen("data.txt", "w");
    if (!write_fp)
    {
        printf("Error: Unable to open file for writing.\n");
        return 1;
    }

    Arr groups[5];
    for (int i = 0; i < 5; i++)
    {
        make(&groups[i]);
    }

    // Input schedules interactively
    input_schedule(write_fp, groups);
    fclose(write_fp);

    FILE *read_fp = fopen("data.txt", "r");
    if (!read_fp)
    {
        printf("Error: Unable to open file for reading.\n");
        return 1;
    }

    // Read schedules from file
    while (1)
    {
        Work *work = (Work *)malloc(sizeof(Work));
        if (fscanf(read_fp, "%d %d %d %d %s %s", &work->data, &work->time, &work->important,
                   &work->group, work->title, work->description) != 6)
        {
            free(work);
            break;
        }
        if (work->group >= 0 && work->group < 5)
        {
            plus(&groups[work->group], work);
        }
    }
    fclose(read_fp);

    int select = 0;
    int total_items = get_total_items(groups);
    int key = 0;

    // Main menu loop
    while (1)
    {
        print_menu(groups, select);

        key = getch();
        if (key == 224)
            key = getch(); // Handle arrow keys

        if (key == UP)
        {
            select = (select - 1 + total_items) % total_items; // Move up
        }
        else if (key == DOWN)
        {
            select = (select + 1) % total_items; // Move down
        }
        else if (key == SPACE)
        {
            break; // Exit on spacebar
        }
    }

    // Free memory
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < groups[i].count; j++)
        {
            free(groups[i].work[j]);
        }
        free(groups[i].work);
    }

    return 0;
}
