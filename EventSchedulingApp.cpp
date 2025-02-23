#include <iostream>
#include <ctime>
using namespace std;

static int uniqueId = 0;

class EventApp
{
public:
    class Event
    {
    public:
        int eventId;
        string name;
        string description;
        string startingTime;
        string endingTime;
        string date;
        int totalMinutes;

        Event(string name, string description, string startingTime, string endingTime, string date, int priority)
        {
            this->eventId = uniqueId++;
            this->name = name;
            this->description = description;
            this->startingTime = startingTime;
            this->endingTime = endingTime;
            this->date = date;

            int day = stoi(date.substr(0, 2));
            int month = stoi(date.substr(3, 5));
            int yearfrom2024 = stoi(date.substr(6)) - 2024;
            int hour = stoi(startingTime.substr(0, 2));
            int minute = stoi(startingTime.substr(3));
            this->totalMinutes = (yearfrom2024 * 525600) + (month * 43800) + (day * 1440) + (hour * 60) + minute;
        }
    };
    struct PriorityQueueNode
    {
        Event *EventByPriority;
        int priority;
        PriorityQueueNode *next;
        PriorityQueueNode(Event *newEvent, int priority)
        {
            this->EventByPriority = newEvent;
            this->priority = priority;
            this->next = NULL;
        }
    } *head;

    struct AVLTreeNode
    {
        Event *EventByPriority;
        int key;
        int height;
        AVLTreeNode *left;
        AVLTreeNode *right;

        AVLTreeNode(Event *node)
        {
            this->EventByPriority = node;
            this->key = node->totalMinutes;
            this->height = 1;
            this->left = NULL;
            this->right = NULL;
        }
    } *root;

    EventApp()
    {
        head = NULL;
        root = NULL;
    }
    int getHeight(AVLTreeNode *node)
    {
        if (node == NULL)
        {
            return 0;
        }
        return node->height;
    }
    int getBalanceFactor(AVLTreeNode *node)
    {
        if (node == NULL)
        {
            return 0;
        }
        return getHeight(node->left) - getHeight(node->right);
    }

    AVLTreeNode *rotateLeft(AVLTreeNode *x)
    {
        AVLTreeNode *y = x->right;
        AVLTreeNode *T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }
    AVLTreeNode *rotateRight(AVLTreeNode *y)
    {
        AVLTreeNode *x = y->left;
        AVLTreeNode *T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }
    AVLTreeNode *minValueNode(AVLTreeNode *node)
    {
        AVLTreeNode *current = node;
        while (current->left != NULL)
        {
            current = current->left;
        }
        return current;
    }
    AVLTreeNode *insertInAVLTree(AVLTreeNode *root, Event *node)
    {
        if (root == NULL)
        {
            return new AVLTreeNode(node);
        }
        if (node->totalMinutes < root->key)
        {
            root->left = insertInAVLTree(root->left, node);
        }
        else if (node->totalMinutes > root->key)
        {
            root->right = insertInAVLTree(root->right, node);
        }
        else
        {
            cout << "Can not create events on the same date and time";
            return root;
        }

        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        int balance = getBalanceFactor(root);

        // Left Left Case
        if (balance > 1 && node->totalMinutes < root->left->key)
        {
            return rotateRight(root);
        }
        // Right Right Case
        if (balance < -1 && node->totalMinutes > root->right->key)
        {
            return rotateLeft(root);
        }
        // Left Right Case
        if (balance > 1 && node->totalMinutes > root->left->key)
        {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        // Right Left Case
        if (balance < -1 && node->totalMinutes < root->right->key)
        {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
        return root;
    }

    AVLTreeNode *removeAVLNode(AVLTreeNode *root, int key, int eventId)
    {
        if (root == NULL)
        {
            return root;
        }

        if (key < root->key)
        {
            root->left = removeAVLNode(root->left, key, eventId);
        }
        else if (key > root->key)
        {
            root->right = removeAVLNode(root->right, key, eventId);
        }
        else
        {
            if (root->EventByPriority->eventId == eventId)
            {
                if ((root->left == NULL) || (root->right == NULL))
                {
                    AVLTreeNode *temp = root->left ? root->left : root->right;

                    if (temp == NULL)
                    {
                        temp = root;
                        root = NULL;
                    }
                    else
                    {
                        *root = *temp;
                    }

                    delete temp;
                }
                else
                {
                    AVLTreeNode *temp = minValueNode(root->right);

                    root->key = temp->key;
                    root->EventByPriority->eventId = temp->EventByPriority->eventId;
                    root->right = removeAVLNode(root->right, temp->key, temp->EventByPriority->eventId);
                }
            }
            else
            {
                return root;
            }
        }

        if (root == NULL)
        {
            return root;
        }
        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        int balance = getBalanceFactor(root);
        if (balance > 1 && getBalanceFactor(root->left) >= 0)
        {
            return rotateRight(root);
        }
        if (balance > 1 && getBalanceFactor(root->left) < 0)
        {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (balance < -1 && getBalanceFactor(root->right) <= 0)
        {
            return rotateLeft(root);
        }
        if (balance < -1 && getBalanceFactor(root->right) > 0)
        {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }

        return root;
    }

    void insertEvent(string name, string description, string startingTime, string endingTime, string date, int priority)
    {
        Event *newEvent = new Event(name, description, startingTime, endingTime, date, priority);
        PriorityQueueNode *newNode = new PriorityQueueNode(newEvent, priority);

        if (checkIfThePriorityQueueContainsSameEvent(newEvent))
        {
            cout << "\n\nCan not create events on the same date and time\n\n";
            delete newEvent; // Prevent memory leak
            delete newNode;  // Prevent memory leak
            return;
        }

        // Insert into priority queue
        if (head == NULL || priority > head->priority)
        {
            newNode->next = head;
            head = newNode;
        }
        else
        {
            PriorityQueueNode *currNode = head;
            while (currNode->next && currNode->next->priority > priority)
            {
                currNode = currNode->next;
            }
            newNode->next = currNode->next;
            currNode->next = newNode;
        }

        // Insert into AVL tree and update root
        root = insertInAVLTree(root, newEvent);
    }

    void removeEvent(int eventId)
    {
        PriorityQueueNode *temp;
        if (head == NULL)
        {
            cout << "\n\nError: There are no currently active events\n";
            return;
        }

        if (head->EventByPriority->eventId == eventId)
        {
            temp = head;
            head = head->next;
            return;
        }

        PriorityQueueNode *currNode = head;
        while (currNode->next && currNode->next->EventByPriority->eventId != eventId)
        {
            currNode = currNode->next;
        }
        temp = currNode->next;

        if (temp == NULL)
        {
            cout << "\n\nError: No such event\n";
        }
        else
        {
            currNode->next = temp->next;
        }
        // remove from AVL
        removeAVLNode(root, temp->EventByPriority->totalMinutes, temp->EventByPriority->eventId);
        delete temp;
    }

    void display(PriorityQueueNode *node)
    {
        if (node == NULL)
        {
            cout << "\n\nError: Event does not exist\n\n";
            return;
        }

        cout << "\n\n***********************************************************************************************\n\n";
        cout << "\n\nEvent Id (" << node->EventByPriority->eventId << ")\nEvent: " << node->EventByPriority->name << "\nDescription: " << node->EventByPriority->description << "\nDate: " << node->EventByPriority->date << "  -  Starting Time: " << node->EventByPriority->startingTime << "  -  Ending Time: " << node->EventByPriority->endingTime;
        cout << "\n\n***********************************************************************************************\n\n";
    }

    void displayEvents()
    {
        if (head == NULL)
        {
            cout << "\n\nError: There are no currently active events\n\n";
            return;
        }

        PriorityQueueNode *currNode = head;
        while (currNode != NULL)
        {
            display(currNode);
            currNode = currNode->next;
        }
    }

    void displayHighestPriorityEvent()
    {
        if (head == NULL)
        {
            cout << "\n\nError: There are no currently active events\n\n";
            return;
        }

        display(head);
    }

    void displayAVlNode(AVLTreeNode *node)
    {
        if (node == NULL)
        {
            cout << "\n\nError: Event does not exist\n\n";
            return;
        }

        cout << "\n\n***********************************************************************************************\n\n";
        cout << "\n\nEvent Id (" << node->EventByPriority->eventId << ")\nEvent: " << node->EventByPriority->name << "\nDescription: " << node->EventByPriority->description << "\nDate: " << node->EventByPriority->date << "  -  Starting Time: " << node->EventByPriority->startingTime << "  -  Ending Time: " << node->EventByPriority->endingTime;
        cout << "\n\n***********************************************************************************************\n\n";
    }
    void displayUpcomingEvent()
    {
        if (root == NULL)
        {
            cout << "\n\nError: There are no currently active events\n\n";
            return;
        }
        AVLTreeNode *upcomingEvent = minValueNode(root);
        displayAVlNode(upcomingEvent);
    }
    void displayEventsAccordingToTime(AVLTreeNode *root)
    {
        if (this->root == NULL)
        {
            cout << "\n\nError: There are no currently active events\n\n";
            return;
        }
        if (root == NULL)
        {
            return;
        }
        displayEventsAccordingToTime(root->left);
        displayAVlNode(root);
        displayEventsAccordingToTime(root->right);
    }
    bool checkIfThePriorityQueueContainsSameEvent(Event *event)
    {
        PriorityQueueNode *currNode = head;
        while (currNode != NULL)
        {
            if (event->date == currNode->EventByPriority->date && event->startingTime == currNode->EventByPriority->startingTime && event->endingTime == currNode->EventByPriority->endingTime)
            {
                return true;
            }
            currNode = currNode->next;
        }
        return false;
    }
};

//***************************************************** METHODS OF MAIN MENU *************************************************************************
void displayMenu()
{
    cout << "\nEvent Scheduling App:\n";
    cout << "1. Create an Event\n";
    cout << "2. Delete an Event by Event ID\n";
    cout << "3. Display All Events\n";
    cout << "4. Display Highest Priority Event\n";
    cout << "5. Display Upcoming Event\n";
    cout << "6. Display Events AccordingToTime\n";
    cout << "7. Exit\n\n";
    cout << "Enter your choice: ";
}

bool isEndTimeValid(const string &startTime, const string &endTime)
{
    int startHour = stoi(startTime.substr(0, 2));
    int startMinute = stoi(startTime.substr(3, 2));
    int endHour = stoi(endTime.substr(0, 2));
    int endMinute = stoi(endTime.substr(3, 2));

    int startTotalMinutes = startHour * 60 + startMinute;
    int endTotalMinutes = endHour * 60 + endMinute;

    return endTotalMinutes > startTotalMinutes;
}

bool isFutureDateTime(string &date, string &time)
{
    time_t now = std::time(0);
    now += 5 * 3600;

    tm *localTime = localtime(&now);

    int currentYear = 1900 + localTime->tm_year;
    int currentMonth = 1 + localTime->tm_mon;
    int currentDay = localTime->tm_mday;
    int currentHour = localTime->tm_hour;
    int currentMinute = localTime->tm_min;

    int eventDay = stoi(date.substr(0, 2));
    int eventMonth = stoi(date.substr(3, 2));
    int eventYear = stoi(date.substr(6, 4));
    int eventHour = stoi(time.substr(0, 2));
    int eventMinute = stoi(time.substr(3, 2));

    if (eventYear > currentYear)
        return true;
    if (eventYear < currentYear)
        return false;

    if (eventMonth > currentMonth)
        return true;
    if (eventMonth < currentMonth)
        return false;

    if (eventDay >= currentDay)
        return true;
    if (eventDay < currentDay)
        return false;

    if (eventHour > currentHour)
        return true;
    if (eventHour < currentHour)
        return false;

    return eventMinute > currentMinute;
}

bool isValidTime(string &time)
{
    try
    {
        if (time.size() != 5 || time[2] != ':')
        {
            return false;
        }
        int hour = stoi(time.substr(0, 2));
        int minute = stoi(time.substr(3, 2));
        return (hour >= 0 && hour < 24 && minute >= 0 && minute < 60);
    }
    catch (invalid_argument)
    {
        return false;
    }
}

bool isValidDate(string &date)
{
    try
    {
        if (date.size() != 10 || date[2] != ':' || date[5] != ':')
        {
            return false;
        }
        int day = stoi(date.substr(0, 2));
        int month = stoi(date.substr(3, 2));
        int year = stoi(date.substr(6, 4));

        if (!(day > 0 && day <= 31 && month > 0 && month <= 12 && year >= 2024))
        {
            return false;
        }
        int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (year % 4 == 0)
            daysInMonth[1] = 29;

        return day <= daysInMonth[month - 1];
    }
    catch (invalid_argument)
    {
        return false;
    }
}

int main()
{
    EventApp eventApp;
    int choice = -1;

    do
    {
        displayMenu();
        string input;
        cin >> input;

        try
        {
            choice = stoi(input);
        }
        catch (invalid_argument)
        {
            choice = -1;
        }

        switch (choice)
        {
        case 1:
        {
            cin.ignore();
            string name, description, startingTime, endingTime, date;
            int priority;

            cout << "\nEnter Event Name: ";
            getline(cin, name);

            cout << "Enter Description: ";
            getline(cin, description);

            do
            {
                cout << "Enter Date (DD:MM:YYYY): ";
                cin >> date;
                if (!isValidDate(date))
                {
                    cout << "Invalid date format or past date!\n";
                    continue;
                }

                cout << "Enter Starting Time (HH:MM): ";
                cin >> startingTime;
                if (!isValidTime(startingTime))
                {
                    cout << "Invalid time format!\n";
                    continue;
                }
                if (!isFutureDateTime(date, startingTime))
                {
                    cout << "Cannot have events in the past!\n";
                    continue;
                }
            } while (!isValidDate(date) || !isValidTime(startingTime) || !isFutureDateTime(date, startingTime));

            do
            {
                cout << "Enter Ending Time (HH:MM): ";
                cin >> endingTime;
                if (!isValidTime(endingTime))
                {
                    cout << "Invalid time format!\n";
                    continue;
                }
                if (!isEndTimeValid(startingTime, endingTime))
                {
                    cout << "End time must be after start time!\n";
                    continue;
                }
            } while (!isValidTime(endingTime) || !isEndTimeValid(startingTime, endingTime));

            do
            {
                cout << "Enter Priority (0=Low, 1=Medium, 2=High, 3=Very High): ";
                string input;
                cin >> input;
                try
                {
                    priority = stoi(input);
                }
                catch (invalid_argument)
                {
                    priority = -1;
                }
                if (priority < 0 || priority > 3)
                {
                    cout << "Invalid priority level!\n";
                }
            } while (priority < 0 || priority > 3);

            eventApp.insertEvent(name, description, startingTime, endingTime, date, priority);
            cout << "\n\n";
            cout << "\n*************************************************************\n";
            break;
        }
        case 2:
        {
            int eventId;
            cout << "\nEnter Event ID to delete: ";
            cin >> eventId;
            eventApp.removeEvent(eventId);
            break;
        }
        case 3:
            cout << "\nAll Events:\n";
            eventApp.displayEvents();
            break;
        case 4:
            cout << "\nHighest Priority Event:\n";
            eventApp.displayHighestPriorityEvent();
            break;
        case 5:
            cout << "\nUpcoming Event:\n";
            eventApp.displayUpcomingEvent();
            break;
        case 6:
            cout << "\nEvents According To Time:\n";
            eventApp.displayEventsAccordingToTime(eventApp.root);
            break;
        case 7:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }

    } while (choice != 7);

    return 0;
}