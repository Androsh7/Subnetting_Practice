#include <iostream>
#include <sstream>
#include <random>

int ipclassarray[3][4] = {
{0  , 128, 192, 224},
{127, 191, 223, 255},
{8  , 16 , 24, 4},
};

char ipclasschar[4] = {'A','B','C','D'};

int octetarray[3][4] = {
{1 , 9 , 17, 25},
{8 , 16, 24, 32},
{1 , 2 , 3 , 4 }
};

int bitvaluearray[5][8] = {
{128, 64 , 32 , 16 , 8  , 4  , 2  , 1  },
{1  , 2  , 3  , 4  , 5  , 6  , 7  , 8  },
{9  , 10 , 11 , 12 , 13 , 14 , 15 , 16 },
{17 , 18 , 19 , 20 , 21 , 22 , 23 , 24 },
{25 , 26 , 27 , 28 , 29 , 30 , 31 , 32 }
};

// Setups the ipaddress class
class ipaddress {
public:
    int ip[4] = { 0,0,0,0 };
    int mask = 0;

    void printRaw() {
        std::cout << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3] << "/" << mask;
    }
    
    // overflow check
    bool overflowcheck() {
        for (int i = 0; i < 4; i++) {
            if (ip[i] > 255 || ip[i] < 0) {
                return true;
            }
        }
        return false;
    }
    
    // ip addition and subtraction
    void ipadd(int iparray[4], int addnum, int octetnum) {
        // adds the amount to the ip array
        iparray[octetnum - 1] = iparray[octetnum - 1] + addnum;

        // makes sure all values are capped at 255
        for (int i = 0; i < 4; i++) {
            if (iparray[i] > 255) {
                iparray[i] = iparray[i] - 256;
                iparray[i + 1] + 1;
            }
        }
        
        // makes sure all values are at least 1
        for (int i = 3; i >= 0; i = i - 1) {
            if (iparray[i] < 0) {
                iparray[i] = iparray[i] + 256;
                iparray[i - 1] = iparray[i - 1] - 1;
            }
        }
    } 
    
    // COB
    // C: class and default mask
    char ipclass;
    int defmask = 0;

    // O: octet number
    int octet = 0;

    // B: bit value
    int bitvalue = 0;

    void determineCOB() {
        //determines the class and default
        for (int i = 0; i < 4; i++) {
            if (ip[0] <= ipclassarray[1][i]) {
                std::cout << "ip:"<< ip[0] << "ipclassarray[][]:" << ipclassarray[1][i] << "\n";
                ipclass = ipclasschar[i];
                defmask = ipclassarray[2][i];
                i = 99;
            }
        }

        // determines the octet number
        for (int i = 0; i < 4; i++) {
            if (mask <= octetarray[1][i]) {
                octet = octetarray[2][i];
                i = 99;
            }
        }

        // determines the bit value
        for (int i = 0; i < 8; i++) {
            if (mask == bitvaluearray[octet][i]) {
                bitvalue = bitvaluearray[0][i];
                i = 99;
            }
        }
    }

    // NFLBX
    // N: network ID
    int netid[4];

    // F: first network
    int fnet[4];

    // L: last network
    int lnet[4];
    
    // B: broadcast
    int bnet[4];

    // X: next network
    int xnet[4];

    void determineNFLBX() {
        
        // copies the current ip to the netid and xnet arrays
        for (int i = 0; i < 4; i++) {
            netid[i] = ip[i];
            xnet[i] = ip[i];
        }
        
        // determines zeros in netid and xnet
        for (int i = 0; i < (4 - octet); i++) {
            netid[3 - i] = 0;
            xnet[3 - i] = 0;
        }
        
        // sets netid and xnet difference
        netid[octet - 1] = round(ip[octet - 1] / bitvalue) * bitvalue;
        xnet[octet - 1] = ((round(ip[octet - 1] / bitvalue)) + 1) * bitvalue;
                                      
        // copies netid to fnet and xnet to bnet and lnet
        for (int i = 0; i < 4; i++) {
            fnet[i] = netid[i];
            bnet[i] = xnet[i];
            lnet[i] = xnet[i];

        }

        // offsets for fnet, lnet, and bnet
        ipadd(fnet, 1, 4);
        ipadd(lnet, -2, 4);
        ipadd(bnet, -1, 4);
        
        // uses ipadd function to prevent values over 255 or below 0
        ipadd(netid, 0, 4);
        ipadd(xnet, 0, 4);
    }

    // Hosts and Networks
    // hosts
    int hosts = 0;

    // networks
    int networks = 0;

    void determineHN() {
        hosts = round(pow(2,(32 - mask))) - 2;
        networks = round( pow(2,(mask - defmask)));
        if (hosts <= 0) {
            hosts = 1;
        }
        if (networks <= 0) {
            networks = 1;
        }
    }
};

int main() {
    
    ipaddress ip1;
    while (true) {
        std::cout << "-----\n";
        std::cout << "Input IPV4 (ex: 192 168 1 1):\n";
        std::cin >> ip1.ip[0];
        std::cin >> ip1.ip[1];
        std::cin >> ip1.ip[2];
        std::cin >> ip1.ip[3];
        std::cout << "Subnet Mask:\n";
        std::cin >> ip1.mask;
        std::cout << "\n";

        ip1.determineCOB();
        std::cout << "Class: " << ip1.ipclass << "/" << ip1.defmask << "\n";
        std::cout << "Octet: " << ip1.octet << "\n";
        std::cout << "Bit value: " << ip1.bitvalue << "\n";

        ip1.determineNFLBX();
        std::cout << "\nNetwork ID: " << ip1.netid[0] << "." << ip1.netid[1] << "." << ip1.netid[2] << "." << ip1.netid[3] << "\n";
        std::cout << "First usable: " << ip1.fnet[0] << "." << ip1.fnet[1] << "." << ip1.fnet[2] << "." << ip1.fnet[3] << "\n";
        std::cout << "Last usable: " << ip1.lnet[0] << "." << ip1.lnet[1] << "." << ip1.lnet[2] << "." << ip1.lnet[3] << "\n";
        std::cout << "Broadcast: " << ip1.bnet[0] << "." << ip1.bnet[1] << "." << ip1.bnet[2] << "." << ip1.bnet[3] << "\n";
        std::cout << "Next network: " << ip1.xnet[0] << "." << ip1.xnet[1] << "." << ip1.xnet[2] << "." << ip1.xnet[3] << "\n";

        ip1.determineHN();
        std::cout << "\nHosts: " << ip1.hosts << "\n";
        std::cout << "Networks: " << ip1.networks << "\n";

    }
}
