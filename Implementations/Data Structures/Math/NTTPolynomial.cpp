const int MAX_DEG = 20, EXP_MULT = 33ULL, EXP = 25;
ull NTTMod = (EXP_MULT << EXP) + 1, NTTBuild[1 << MAX_DEG][2], tempPVPoly[1 << MAX_DEG], rt[(1 << MAX_DEG) + 1];

ull modPow(ull a, ull p) {
    if(!p) return 1;
    if(p & 1) return a * modPow(a, p ^ 1) % NTTMod;
    ull sqrt = modPow(a, p >> 1);
    return SQ(sqrt) % NTTMod;
}

ull findCyclic() {
    vi multFactors;
    ull temp = EXP_MULT;
    for(int i = 2; i * i <= temp; i += 2) {
        if(temp % i == 0) multFactors.PB(i);
        while(temp % i == 0) temp /= i;
        if(i == 2) i--;
    }
    if(temp > 1) multFactors.PB(temp);
    FOR(i, 2, NTTMod) {
        bool works = 1;
        if(modPow(i, NTTMod >> 1) == 1) works = 0;
        for(const int factor : multFactors) if(modPow(i, NTTMod / factor) == 1) works = 0;
        if(works) return i;
    }
    return -1;
}

void buildRT() {
    if(rt[0] == 1) return;
    rt[0] = 1;
    rt[1] = modPow(findCyclic(), EXP_MULT << (EXP - MAX_DEG));
    FOR(i, 2, (1 << MAX_DEG) + 1) rt[i] = rt[i - 1] * rt[1] % NTTMod;
}

struct Polynomial {

    int deg;
    ull *coefficients;

    Polynomial(int d = 0) {
        deg = d;
        coefficients = new ull[deg + 1];
        memset(coefficients, 0, (deg + 1) * sizeof(ull));
        buildRT();
    }

    void NTT(int neededDeg) {
        R0F(i, neededDeg + 1) {
            int arr = i & 1, narr = arr ^ 1, lli = 1 << i, llil = lli << 1, llndim1 = 1 << (neededDeg - i - 1), rtp = lli << (MAX_DEG - neededDeg);
            if(i == neededDeg) {
                F0R(j, deg + 1) NTTBuild[j][arr] = coefficients[j];
                FOR(j, deg + 1, lli) NTTBuild[j][arr] = 0;
            } else F0R(j, lli) {
                int x = j, y = 0, z = j, w = j + lli;
                F0R(k, llndim1) {
                    NTTBuild[x][arr] = (NTTBuild[z][narr] + NTTBuild[w][narr] * rt[y]) % NTTMod;
                    x += lli, y += rtp, z += llil, w += llil;
                }
                z = j, w = j + lli;
                F0R(k, llndim1) {
                    NTTBuild[x][arr] = (NTTBuild[z][narr] + NTTBuild[w][narr] * rt[y]) % NTTMod;
                    x += lli, y += rtp, z += llil, w += llil;
                }
            }
        }
    }

    static void invNTT(ull *p, int pdeg) {
        int mp = 1 << MAX_DEG;
        ull u = modPow(1 << pdeg, NTTMod - 2);
        R0F(i, pdeg + 1) {
            int arr = i & 1, narr = arr ^ 1, lli = 1 << i, llil = lli << 1, llpdim1 = 1 << (pdeg - i - 1), rtp = lli << (MAX_DEG - pdeg);
            R0F(j, lli) {
                if(i == pdeg) NTTBuild[j][arr] = p[j] * u % NTTMod;
                else {
                    int x = j, y = mp, z = j, w = lli + j;
                    F0R(k, llpdim1) {
                        NTTBuild[x][arr] = (NTTBuild[z][narr] + NTTBuild[w][narr] * rt[y]) % NTTMod;
                        x += lli, y -= rtp, z += llil, w += llil;
                    }
                    z = j, w = lli + j;
                    F0R(k, llpdim1) {
                        NTTBuild[x][arr] = (NTTBuild[z][narr] + NTTBuild[w][narr] * rt[y]) % NTTMod;
                        x += lli, y -= rtp, z += llil, w += llil;
                    }
                }
            }
        }
    }

    Polynomial operator ()(Polynomial p) {
        Polynomial curr;
        curr[0] = 1;
        Polynomial res;
        F0R(i, deg + 1) {
            res += curr * coefficients[i];
            curr *= p;
        }
        return res;
    }

    ull& operator [](int idx) {
        return coefficients[idx];
    }

    Polynomial operator +(ull a) {
        Polynomial p(deg);
        F0R(i, deg + 1) p[i] = coefficients[i];
        p[0] += a;
        p[0] %= NTTMod;
        return p;
    }

    Polynomial operator -(ull a) {
        Polynomial p(deg);
        F0R(i, deg + 1) p[i] = coefficients[i];
        p[0] += NTTMod - a;
        p[0] %= NTTMod;
        return p;
    }

    Polynomial operator *(ull mult) {
        Polynomial p(deg);
        F0R(i, deg + 1) p[i] = coefficients[i] * mult % NTTMod;
        return p;
    }

    Polynomial operator /(ull div) {
        return *this * modPow(div, NTTMod - 2);
    }

    Polynomial operator +(Polynomial b) {
        Polynomial res(max(deg, b.deg));
        F0R(i, max(deg, b.deg) + 1) res[i] = (((i > deg) ? 0 : coefficients[i]) + ((i > b.deg) ? 0 : b[i])) % NTTMod;
        return res;
    }

    Polynomial operator -(Polynomial p) {
        return *this + (p * (NTTMod - 1));
    }

    Polynomial operator *(Polynomial b) {
        int neededDeg = 32 - __builtin_clz(deg + b.deg);
        NTT(neededDeg);
        F0R(i, (1 << neededDeg)) tempPVPoly[i] = NTTBuild[i][0];
        b.NTT(neededDeg);
        F0R(i, (1 << neededDeg)) tempPVPoly[i] = tempPVPoly[i] * NTTBuild[i][0] % NTTMod;
        invNTT(tempPVPoly, neededDeg);
        Polynomial res(deg + b.deg);
        F0R(i, res.deg + 1) res[i] = NTTBuild[i][0];
        return res;
    }

    void operator +=(ull a) {
        coefficients[0] += a;
        coefficients[0] %= NTTMod;
    }

    void operator -=(ull a) {
        *this += NTTMod - a;
    }

    void operator *=(ull mult) {
        F0R(i, deg + 1) coefficients[i] *= mult, coefficients[i] %= NTTMod;
    }

    void operator /=(ull div) {
        *this *= modPow(div, NTTMod - 2);
    }

    void operator +=(Polynomial b) {
        *this = *this + b;
    }

    void operator -=(Polynomial p) {
        *this = *this - p;
    }

    void operator *=(Polynomial b) {
        *this = *this * b;
    }

};
