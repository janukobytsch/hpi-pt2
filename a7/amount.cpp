#include <assert.h>

class Amount {
public:
    enum class Currency { EURO, DOLLAR };

	Amount(): _grossValue{0}, _taxRatio{0}, _currency{Currency::EURO} {};
    Amount(double grossValue): _grossValue{grossValue}, _taxRatio{0}, _currency{Currency::EURO} {};
    Amount(double grossValue, double taxRatio): _grossValue{grossValue}, _taxRatio{taxRatio}, _currency{Currency::EURO} {};

    Amount(const Amount &a);
    Amount& operator= (const Amount &a);

    bool operator!();

    friend bool operator==(Amount v1, Amount v2);
    friend bool operator!=(Amount v1, Amount v2);

    friend Amount operator+(const Amount &v1, const Amount &v2);
    friend Amount operator-(const Amount &v1, const Amount &v2);

    Amount operator+=(const Amount &v);
    Amount operator-=(const Amount &v);

    void toEuro();
    void toDollar();
    void convert();
    double getTax(); /* calculate dynamically */
    double netValue(); /* calculate dynamically */
    double grossValue() const {
        return _grossValue;
    };
    double getTaxRatio() const {
        return _taxRatio;
    }
    void setTaxRatio(double taxRatio) {
        Amount::_taxRatio = taxRatio;
    }
    Currency const &getCurrency() const {
        return _currency;
    }
    double getExchangeRate() const {
        return _exchangeRate;
    }
    void setExchangeRate(double exchangeRate) {
        _exchangeRate = exchangeRate;
    }

private:
    double _taxRatio; /* considered for calculation of net value */
    //double _taxAmount; /* calculated dynamically based on grossValue and taxRatio */
    //double _netValue; /* calculated dynamically based on grossValue and taxRatio */
    double _grossValue; /* base value */
    Currency _currency;
    static double _exchangeRate; /* fixed rate between currencies */
};

Amount::Amount(const Amount &a) {
    _taxRatio = a._taxRatio;
    _grossValue = a._grossValue;
    _currency = a._currency;
}

Amount &Amount::operator=(const Amount &a) {
    // check for self-assignment
    if (this == &a) {
        return *this;
    }
    _taxRatio = a._taxRatio;
    _grossValue = a._grossValue;
    _currency = a._currency;
    return *this;
}

bool Amount::operator!() {
    return _grossValue == 0;
}

// TODO autoconvert currencies for arithmetics

Amount Amount::operator+=(const Amount &a) {
    assert(_currency == a._currency);
    _grossValue += a._grossValue;
    return *this;
}

Amount Amount::operator-=(const Amount &a) {
    assert(_currency == a._currency);
    _grossValue -= a._grossValue;
    return *this;
}

bool operator==(Amount v1, Amount v2) {
    return (v1._grossValue == v2._grossValue &&
        v1._currency == v2._currency);
}

bool operator!=(Amount v1, Amount v2) {
    return !(v1 == v2);
}

Amount operator+(const Amount &a1, const Amount &a2) {
    assert(a1._currency == a2._currency);
    return Amount{a1._grossValue + a2._grossValue};
}

Amount operator-(const Amount &a1, const Amount &a2) {
    assert(a1._currency == a2._currency);
    return Amount{a1._grossValue - a2._grossValue};
}

double Amount::getTax() {
    return _grossValue * _taxRatio;
}

double Amount::netValue() {
    return _grossValue * (1 - _taxRatio);
}

void Amount::toEuro() {
    if (_currency == Currency::EURO) {
        return;
    }
    _grossValue = _grossValue / _exchangeRate;
    _currency = Currency::EURO;
}

void Amount::toDollar() {
    if (_currency == Currency::DOLLAR) {
        return;
    }
    _grossValue = _grossValue * _exchangeRate;
    _currency = Currency::DOLLAR;
}

void Amount::convert() {
    if (_currency == Currency::DOLLAR) {
        toEuro();
    } else {
        toDollar();
    }
}

// set fixed exchange rate
double Amount::_exchangeRate = 1.15;

void test() {
    // test default constructor
    Amount a1;
    a1.setExchangeRate(1.15);
    assert(a1.netValue() == 0.0 && a1.grossValue() == 0.0);

    // test default currency
    assert(a1.getCurrency() == Amount::Currency::EURO);

    // test init-list
    Amount a2 {100, 0.19};
    assert(a2.grossValue() == 100 && a2.getTaxRatio() == 0.19);

    // test assignment
    Amount a3;
    a3 = a2;
    assert(a3.grossValue() == a2.grossValue() && a3.getTaxRatio() == a2.getTaxRatio());

    // test copy-constructor
    Amount a4 = a3;
    assert(a4.grossValue() == a3.grossValue() && a4.getTaxRatio() == a3.getTaxRatio());

    // test algebraic operators
    Amount a5 {250};
    Amount a6 {120};
    Amount a7 = a5 + a6;
    assert(a7.grossValue() == 370);
    Amount a8 = a5 - a6;
    assert(a8.grossValue() == 130);

    // test calculated values
    Amount a9 {100};
    a9.setTaxRatio(0.19);
    assert(a9.getTaxRatio() == 0.19 && a9.netValue() == 81.0 && a9.getTax() == 19);

    // test autoconvert
    Amount a10(a9);
    a9.toDollar();
    assert(a9.getCurrency() == Amount::Currency::DOLLAR && a10.getCurrency() == Amount::Currency::EURO
        && a9.grossValue() > a10.grossValue());
    a9.toEuro();
    assert(a9.getCurrency() == Amount::Currency::EURO && a10.getCurrency() == Amount::Currency::EURO
        && a9.grossValue() == a10.grossValue());
}

int main() {
	test();
    return 0;
}
