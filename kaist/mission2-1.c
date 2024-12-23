#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <unistd.h> // ftruncate 함수를 위한 헤더

#define SPACE 32
#define UP 72
#define DOWN 80
#define QUIT 'q'
#define INSERT 'i'
#define Lsat 'e'
typedef struct
{
    int data;
    int time;
    int important;
    int group;
    char title[128];
    char description[128];
} Work;

typedef struct
{
    Work **work;
    int count;
    int nowmax;
} Arr;

const char *GROUP_NAMES[] = {"공부", "상담", "약속", "휴식", "기타"};

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
    sprintf(formatted, "%d.%02d.%02d.", date / 10000, (date % 10000) / 100, date % 100);
}

void format_time(int time, char *formatted)
{
    sprintf(formatted, "%02d:%02d", time / 100, time % 100);
}

void clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void print_item(Work *work, int is_selected)
{
    char formatted_date[15];
    char formatted_time[6];
    format_date(work->data, formatted_date);
    format_time(work->time, formatted_time);

    printf("%s%-14s %-8s %-8d %-8s %-16s\n",
           is_selected ? "▶" : " ",
           formatted_date, formatted_time, work->important,
           GROUP_NAMES[work->group], work->title);
}

void print_menu(Arr *groups, int selected)
{
    clear_screen();
    printf("\n%-14s %-8s %-8s %-8s %-16s\n", "날짜", "시간", "중요도", "그룹", "제목");
    printf("----------------------------------------------------------------\n");

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

int get_total_items(Arr *groups)
{
    int total = 0;
    for (int i = 0; i < 5; i++)
    {
        total += groups[i].count;
    }
    return total;
}

void print_details(Work *work)
{
    char formatted_date[15], formatted_time[6];
    format_date(work->data, formatted_date);
    format_time(work->time, formatted_time);

    clear_screen();
    printf("일시: %s %s\n", formatted_date, formatted_time);
    printf("중요도: %d\n", work->important);
    printf("그룹: %s\n", GROUP_NAMES[work->group]);
    printf("제목: %s\n", work->title);
    printf("내용: %s\n", work->description);
    printf("\n'q'를 눌러 목록으로 돌아가기\n");
}

void edit_schedule(Work *work, FILE *fp, Arr *groups)
{
    char input[128];
    int temp;

    printf("adit now.\n");

    printf("date (%d): ", work->data);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0)
    {
        sscanf(input, "%d", &work->data);
    }

    printf("time (%d): ", work->time);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0)
    {
        sscanf(input, "%d", &work->time);
    }

    printf("impo (%d): ", work->important);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0)
    {
        sscanf(input, "%d", &work->important);
    }

    printf("gro (%s): ", GROUP_NAMES[work->group]);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0)
    {
        sscanf(input, "%d", &temp);
        if (temp >= 0 && temp < 5)
        {
            // 그룹이 변경되면 배열에서의 위치도 변경
            if (temp != work->group)
            {
                // 이전 그룹에서 제거
                for (int i = 0; i < groups[work->group].count; i++)
                {
                    if (groups[work->group].work[i] == work)
                    {
                        for (int j = i; j < groups[work->group].count - 1; j++)
                        {
                            groups[work->group].work[j] = groups[work->group].work[j + 1];
                        }
                        groups[work->group].count--;
                        break;
                    }
                }
                // 새 그룹에 추가
                work->group = temp;
                plus(&groups[work->group], work);
            }
        }
    }

    printf("title (%s): ", work->title);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0)
    {
        strcpy(work->title, input);
    }

    printf("mosimosi (%s): ", work->description);
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0)
    {
        strcpy(work->description, input);
    }

    // 파일 업데이트
    fseek(fp, 0, SEEK_SET);
    ftruncate(fileno(fp), 0); // 파일 내용 초기화

    // 모든 일정을 파일에 다시 쓰기
    for (int g = 0; g < 5; g++)
    {
        for (int i = 0; i < groups[g].count; i++)
        {
            fprintf(fp, "%d %d %d %d %s %s\n",
                    groups[g].work[i]->data,
                    groups[g].work[i]->time,
                    groups[g].work[i]->important,
                    groups[g].work[i]->group,
                    groups[g].work[i]->title,
                    groups[g].work[i]->description);
        }
    }
    fflush(fp); // 파일 변경사항 즉시 저장

    printf("true!\n");
    getch();
}
void add_schedule(Arr *groups, FILE *fp)
{
    Work *new_work = (Work *)malloc(sizeof(Work));
    printf("일정을 추가합니다.\n");
    printf("날짜를 입력하세요 (YYYYMMDD): ");
    scanf("%d", &new_work->data);
    printf("시간을 입력하세요 (HHMM): ");
    scanf("%d", &new_work->time);
    printf("중요도를 입력하세요: ");
    scanf("%d", &new_work->important);
    printf("그룹을 입력하세요 (0:공부, 1:상담, 2:약속, 3:휴식, 4:기타): ");
    scanf("%d", &new_work->group);
    printf("제목을 입력하세요: ");
    scanf("%s", new_work->title);
    printf("내용을 입력하세요: ");
    scanf("%s", new_work->description);

    fprintf(fp, "%d %d %d %d %s %s\n",
            new_work->data, new_work->time, new_work->important,
            new_work->group, new_work->title, new_work->description);

    plus(&groups[new_work->group], new_work);
    printf("일정이 추가되었습니다!\n");
    getch();
}

int main()
{
    FILE *fp = fopen("data.txt", "r+");
    if (!fp)
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
        if (fscanf(fp, "%d %d %d %d %s %s",
                   &work->data, &work->time, &work->important,
                   &work->group, work->title, work->description) != 6)
        {
            free(work);
            break;
        }
        if (work->group < 5)
        {
            plus(&arr[work->group], work);
        }
    }

    int select = 0, key = 0;
    int total_items = get_total_items(arr);

    while (1)
    {
        print_menu(arr, select);

        key = getch();
        if (key == 224)
            key = getch();

        if (key == UP)
        {
            select = (select - 1 + total_items) % total_items;
        }
        else if (key == DOWN)
        {
            select = (select + 1) % total_items;
        }
        else if (key == SPACE)
        {
            Work *selected_work = NULL;
            int current_item = 0;
            for (int g = 0; g < 5; g++)
            {
                for (int i = 0; i < arr[g].count; i++)
                {
                    if (current_item == select)
                    {
                        selected_work = arr[g].work[i];
                        break;
                    }
                    current_item++;
                }
                if (selected_work)
                    break;
            }

            if (selected_work)
            {
                while (1)
                {
                    print_details(selected_work);
                    key = getch();
                    if (key == QUIT)
                        break;
                    else if (key == Lsat)
                    { // 'e' 키를 눌렀을 때
                        edit_schedule(selected_work, fp, arr);
                    }
                }
            }
        }
        else if (key == INSERT)
        {
            add_schedule(arr, fp);
            total_items = get_total_items(arr);
        }
    }

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < arr[i].count; j++)
        {
            free(arr[i].work[j]);
        }
        free(arr[i].work);
    }

    fclose(fp);
    return 0;
}
