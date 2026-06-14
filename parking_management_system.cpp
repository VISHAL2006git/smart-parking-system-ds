#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <iomanip>
#include <cctype>
#include <vector>
using namespace std;

const int ROWS = 25;
const int COLS = 20;
const int TOTAL = ROWS * COLS;
const int A_SIZE = 100, B_SIZE = 200, C_SIZE = 200;
const char *HIST_FILE = "history.txt";
const char *CSV_FILE = "history.csv";
const char *VIP_FILE = "vipid.txt";
const char *STAFF_FILE = "staffid.txt";

struct Vehicle {
    bool occ;
    string vno, owner, uid;
    char zone;
    time_t inT;
    int slot;
    Vehicle() : occ(false), zone('-'), inT(0), slot(-1) {}
};
struct Hist {
    string act, vno, owner, uid, dur;
    char zone;
    int slot;
    time_t inT, outT;
    Hist() : zone('-'), slot(-1), inT(0), outT(0), dur("00:00:00") {}
};
struct HStack {
    Hist a[2000];
    int t;
    HStack() : t(-1) {}
    void init() { t = -1; }
    void push(const Hist &h) { if (t < 1999) a[++t] = h; }
};

Vehicle lot[TOTAL];
bool inHeap[TOTAL];
HStack hist;
string vipArr[1000], staffArr[1000];
int vipN = 0, staffN = 0;

struct MinHeap {
    int a[520], n, s, e;
    void init(int ss, int ee) { n = 0; s = ss; e = ee; }
    bool empty() { return n == 0; }
    void push(int slot) {
        a[++n] = slot;
        for (int i = n; i > 1 && a[i] < a[i/2]; i /= 2) swap(a[i], a[i/2]);
    }
    int pop() {
        if (n < 1) return -1;
        int res = a[1];
        a[1] = a[n--];
        for (int i = 1;;) {
            int l = i*2, r = i*2+1, sm = i;
            if (l <= n && a[l] < a[sm]) sm = l;
            if (r <= n && a[r] < a[sm]) sm = r;
            if (sm != i) swap(a[i], a[sm]), i = sm;
            else break;
        }
        return res;
    }
} hA, hB, hC;

int readInt() {
    int x;
    while (!(cin >> x)) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "Invalid number.\n";
    }
    cin.ignore(1000, '\n');
    return x;
}
string readLine() {
    string s; getline(cin, s);
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
string timeStr(time_t t) {
    if (!t) return "-";
    char buf[32]; strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return buf;
}
string formatDuration(long long s) {
    if (s < 0) s = 0;
    int h = s / 3600, m = (s % 3600) / 60, se = s % 60;
    char buf[16]; sprintf(buf, "%02d:%02d:%02d", h, m, se);
    return buf;
}
bool validVno(const string &s) {
    bool a = 0, d = 0;
    for (char c : s) { if (isalpha(c)) a = 1; if (isdigit(c)) d = 1; }
    return a && d;
}
bool validName(const string &s) {
    if (s.empty()) return 0;
    for (char c : s) if (!(isalpha(c) || c == ' ')) return 0;
    return 1;
}
string upcase(string s) {
    for (char &c : s) if (c >= 'a' && c <= 'z') c -= 32;
    return s;
}
void loadIDs() {
    vipN = staffN = 0;
    ifstream v(VIP_FILE);
    string l;
    while (v && getline(v, l)) if (!l.empty()) vipArr[vipN++] = l;
    v.close();
    ifstream s(STAFF_FILE);
    while (s && getline(s, l)) if (!l.empty()) staffArr[staffN++] = l;
    s.close();
}
bool addID(const string &id, char k) {
    if (k == 'A') {
        for (int i = 0; i < vipN; i++) if (vipArr[i] == id) return 0;
        ofstream o(VIP_FILE, ios::app); o << id << '\n'; o.close();
        vipArr[vipN++] = id; return 1;
    } else {
        for (int i = 0; i < staffN; i++) if (staffArr[i] == id) return 0;
        ofstream o(STAFF_FILE, ios::app); o << id << '\n'; o.close();
        staffArr[staffN++] = id; return 1;
    }
}
void appendHistFile(const Hist &h) {
    ofstream o(HIST_FILE, ios::app);
    o << h.act << '|' << h.vno << '|' << h.owner << '|' << h.zone << '|'
      << h.slot << '|' << h.uid << '|' << (long long)h.inT << '|'
      << (long long)h.outT << '|' << h.dur << '\n';
    o.close();
}
void exportCSV() {
    ifstream in(HIST_FILE);
    if (!in) { cout << "No history records to export.\n"; return; }
    ofstream o(CSV_FILE);
    if (!o) { cout << "Cannot create CSV file.\n"; in.close(); return; }
    o << "Action,Vehicle,Owner,Zone,Slot,UserID,Entry,Exit,Duration\n";
    string l; bool any = 0;
    while (getline(in, l)) {
        if (l.empty()) continue;
        any = 1;
        string p[9]; int k = 0; size_t pr = 0, po;
        while (k < 9 && (po = l.find('|', pr)) != string::npos) {
            p[k++] = l.substr(pr, po - pr); pr = po + 1;
        }
        p[k++] = l.substr(pr);
        o << p[0] << "," << p[1] << "," << p[2] << "," << p[3] << "," << p[4]
          << "," << p[5] << "," << p[6] << "," << p[7] << "," << p[8] << '\n';
    }
    in.close(); o.close();
    if (!any) cout << "No history records to export.\n";
    else cout << "Exported history to " << CSV_FILE << "\n";
}
void initAll() {
    for (int i = 0; i < TOTAL; i++) lot[i] = Vehicle(), inHeap[i] = 0;
    int as = 0, ae = A_SIZE - 1, bs = ae + 1, be = bs + B_SIZE - 1, cs = be + 1, ce = cs + C_SIZE - 1;
    hA.init(as, ae); hB.init(bs, be); hC.init(cs, ce);
    for (int i = as; i <= ae; i++) hA.push(i), inHeap[i] = 1;
    for (int i = bs; i <= be; i++) hB.push(i), inHeap[i] = 1;
    for (int i = cs; i <= ce; i++) hC.push(i), inHeap[i] = 1;
    hist.init(); loadIDs();
    cout << "System initialized.\nVIP IDs: " << vipN << " Staff IDs: " << staffN << "\n";
}
int allocFromZone(char z) {
    if (z == 'A' && !hA.empty()) { int s = hA.pop(); inHeap[s] = 0; return s; }
    if (z == 'B' && !hB.empty()) { int s = hB.pop(); inHeap[s] = 0; return s; }
    if (z == 'C' && !hC.empty()) { int s = hC.pop(); inHeap[s] = 0; return s; }
    return -1;
}
void freeToZone(int s) {
    if (s < 0 || s >= TOTAL || lot[s].occ || inHeap[s]) return;
    if (s >= hA.s && s <= hA.e) hA.push(s), inHeap[s] = 1;
    else if (s >= hB.s && s <= hB.e) hB.push(s), inHeap[s] = 1;
    else if (s >= hC.s && s <= hC.e) hC.push(s), inHeap[s] = 1;
}
void parkFlow() {
    cout << "\nPark Vehicle\nEnter type (VIP/Staff/General):\n";
    string t = readLine(); while (t.empty()) { cout << "Enter type:\n"; t = readLine(); }
    string up = upcase(t); char z = 'C'; string id = "";
    if (up == "VIP") {
        z = 'A'; cout << "Enter VIP ID:\n"; id = readLine();
        bool ok = 0; for (int i = 0; i < vipN; i++) if (vipArr[i] == id) ok = 1;
        if (!ok) { cout << "Unknown ID. Register? (y/n):\n"; string r = readLine();
            if (!r.empty() && (r[0] == 'y' || r[0] == 'Y')) addID(id, 'A'); else return; }
    } else if (up == "STAFF") {
        z = 'B'; cout << "Enter Staff ID:\n"; id = readLine();
        bool ok = 0; for (int i = 0; i < staffN; i++) if (staffArr[i] == id) ok = 1;
        if (!ok) { cout << "Unknown ID. Register? (y/n):\n"; string r = readLine();
            if (!r.empty() && (r[0] == 'y' || r[0] == 'Y')) addID(id, 'S'); else return; }
    } else {
        cout << "Choose: 1) Staff 2) General 0) Auto\n";
        int c = readInt(); if (c == 1) z = 'B'; else if (c == 2) z = 'C';
        else z = (hB.n >= hC.n) ? 'B' : 'C';
    }
    cout << "Enter Vehicle Number:\n"; string v = readLine();
    while (!validVno(v)) { cout << "Invalid.\n"; v = readLine(); }
    cout << "Enter Owner Name:\n"; string n = readLine();
    while (!validName(n)) { cout << "Invalid.\n"; n = readLine(); }
    int s = allocFromZone(z);
    if (s < 0) { cout << "Zone full.\n"; return; }
    lot[s].occ = 1; lot[s].vno = v; lot[s].owner = n; lot[s].uid = id; lot[s].zone = z; lot[s].slot = s; lot[s].inT = time(0);
    Hist h; h.act = "IN"; h.vno = v; h.owner = n; h.uid = id; h.zone = z; h.slot = s; h.inT = lot[s].inT; h.outT = 0;
    hist.push(h); appendHistFile(h);
    cout << "Parked at slot " << s << " Zone " << z << "\nEntry: " << timeStr(h.inT) << "\n";
}
void removeFlow() {
    cout << "\nRemove Vehicle\n1) Vehicle No 2) Slot No\n";
    int c = readInt(); int f = -1;
    if (c == 1) {
        cout << "Enter Vehicle No:\n"; string v = readLine();
        for (int i = 0; i < TOTAL; i++) if (lot[i].occ && lot[i].vno == v) f = i;
        if (f == -1) { cout << "Not found.\n"; return; }
    } else {
        cout << "Enter Slot:\n"; int s = readInt();
        if (s < 0 || s >= TOTAL || !lot[s].occ) { cout << "Invalid slot.\n"; return; }
        f = s;
    }
    time_t outt = time(0); long long dur = difftime(outt, lot[f].inT);
    string durS = formatDuration(dur);
    cout << "Removed " << lot[f].vno << " Slot " << f << " Zone " << lot[f].zone << "\n";
    cout << "In:" << timeStr(lot[f].inT) << "\nOut:" << timeStr(outt) << "\nDur:" << durS << "\n";
    Hist h; h.act = "OUT"; h.vno = lot[f].vno; h.owner = lot[f].owner; h.uid = lot[f].uid;
    h.zone = lot[f].zone; h.slot = f; h.inT = lot[f].inT; h.outT = outt; h.dur = durS;
    hist.push(h); appendHistFile(h);
    lot[f] = Vehicle(); freeToZone(f);
    cout << "Slot freed.\n";
}
void showAll() {
    cout << "\nOccupied Vehicles:\n";
    bool any = 0;
    cout << left << setw(6) << "Slot" << setw(14) << "Vehicle" << setw(18) << "Owner" << setw(6) << "Zone" << setw(20) << "Entry" << "\n";
    for (int i = 0; i < TOTAL; i++)
        if (lot[i].occ) {
            any = 1;
            cout << left << setw(6) << i << setw(14) << lot[i].vno << setw(18)
                 << lot[i].owner << setw(6) << lot[i].zone << setw(20) << timeStr(lot[i].inT) << "\n";
        }
    if (!any) cout << "No vehicles parked.\n";
}
void basicSearch() {
    cout << "Vehicle No:\n"; string v = readLine(); bool f = 0;
    for (int i = 0; i < TOTAL; i++)
        if (lot[i].occ && lot[i].vno == v) {
            f = 1; cout << "Found Slot " << i << " Zone " << lot[i].zone << "\n";
            cout << "Owner:" << lot[i].owner << "\nEntry:" << timeStr(lot[i].inT) << "\n";
        }
    if (!f) cout << "Not found.\n";
}
void showHistory() {
    ifstream in(HIST_FILE); if (!in) { cout << "No history.\n"; return; }
    vector<string> v; string l; while (getline(in, l)) if (!l.empty()) v.push_back(l);
    in.close(); if (v.empty()) { cout << "No history.\n"; return; }
    for (int i = v.size() - 1; i >= 0; i--) {
        string p[9]; int k = 0; size_t pr = 0, po;
        while (k < 9 && (po = v[i].find('|', pr)) != string::npos) { p[k++] = v[i].substr(pr, po - pr); pr = po + 1; }
        p[k++] = v[i].substr(pr);
        cout << p[0] << " | " << p[1] << " | " << p[2] << " | Zone:" << p[3] << " Slot:" << p[4]
             << " UID:" << p[5] << "\nIn:" << timeStr((time_t)atoll(p[6].c_str()))
             << "\nOut:" << (p[7] == "0" ? "-" : timeStr((time_t)atoll(p[7].c_str())))
             << "\nDur:" << p[8] << "\n-------------------------\n";
    }
}
void showStats() {
    int o = 0, a = 0, b = 0, c = 0;
    for (int i = 0; i < TOTAL; i++)
        if (lot[i].occ) { o++; if (i <= hA.e) a++; else if (i <= hB.e) b++; else c++; }
    cout << "Total:" << TOTAL << "\nOcc:" << o << "\nEmpty:" << TOTAL - o
         << "\nA:" << a << "/" << A_SIZE << " B:" << b << "/" << B_SIZE
         << " C:" << c << "/" << C_SIZE << "\n";
}
void menu() {
    while (1) {
        cout << "\n=== MENU ===\n1)Park\n2)Remove\n3)Display\n4)Search\n5)History\n6)Stats\n7)Export\n8)Exit\n";
        int c = readInt();
        if (c == 1) parkFlow();
        else if (c == 2) removeFlow();
        else if (c == 3) showAll();
        else if (c == 4) basicSearch();
        else if (c == 5) showHistory();
        else if (c == 6) showStats();
        else if (c == 7) exportCSV();
        else if (c == 8) {
            cout << "Exiting program.\n";
            break;
        } else cout << "Invalid choice.\n";
    }
}

int main() {
    initAll();
    menu();
    return 0;
}