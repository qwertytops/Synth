using namespace std;

class LPF {
public:
    double run(double input);

    LPF(double alpha);

private:
    double alpha;
    double previous;
    bool init;
};