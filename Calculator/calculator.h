#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>

namespace Ui {
class Calculator;
}

class Calculator : public QWidget
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator() override;

private:
    enum class State {
        EnteringFirst,   // 正在输入第一个操作数
        OperatorPending, // 刚按下运算符，等待第二个操作数
        EnteringSecond,  // 正在输入第二个操作数
        ShowingResult    // 已显示计算结果
    };

    void appendDigit(const QString &digit);
    void appendDecimal();
    void applyOperator(const QString &op);
    void calculate();
    void backspace();
    void clearAll();
    void toggleSign();

    void setDisplay(const QString &text);
    void setExpression(const QString &text);

    Ui::Calculator *ui;

    State m_state = State::EnteringFirst;
    QString m_operand;
    double m_firstOperand = 0.0;
    QString m_pendingOperator;
};

#endif // CALCULATOR_H
