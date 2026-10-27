#include "calculator.h"
#include "ui_calculator.h"

#include <QPushButton>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);

    // 数字键统一按文字识别，避免为十个按钮各写一遍 connect
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
        const QString text = button->text();
        if (text.size() == 1 && text.at(0).isDigit()) {
            connect(button, &QPushButton::clicked, this, [this, text] {
                appendDigit(text);
            });
        }
    }

    connect(ui->btnDecimal, &QPushButton::clicked, this, &Calculator::appendDecimal);
    connect(ui->btnAdd, &QPushButton::clicked, this, [this] { applyOperator("+"); });
    connect(ui->btnSubtract, &QPushButton::clicked, this, [this] { applyOperator("-"); });
    connect(ui->btnMultiply, &QPushButton::clicked, this, [this] { applyOperator("×"); });
    connect(ui->btnDivide, &QPushButton::clicked, this, [this] { applyOperator("÷"); });
    connect(ui->btnEquals, &QPushButton::clicked, this, &Calculator::calculate);
    connect(ui->btnClear, &QPushButton::clicked, this, &Calculator::clearAll);
    connect(ui->btnBackspace, &QPushButton::clicked, this, &Calculator::backspace);
    connect(ui->btnSign, &QPushButton::clicked, this, &Calculator::toggleSign);

    clearAll();
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::appendDigit(const QString &digit)
{
    if (m_state == State::Error)
        clearAll();

    if (m_state == State::ShowingResult) {
        m_operand.clear();
        m_state = State::EnteringFirst;
        setExpression(QString());
    }
    if (m_state == State::OperatorPending) {
        m_operand.clear();
        m_state = State::EnteringSecond;
    }

    if (m_operand == "0")
        m_operand = digit;
    else
        m_operand += digit;

    setDisplay(m_operand);
}

void Calculator::appendDecimal()
{
    if (m_state == State::Error)
        clearAll();

    if (m_state == State::ShowingResult) {
        m_operand.clear();
        m_state = State::EnteringFirst;
        setExpression(QString());
    }
    if (m_state == State::OperatorPending) {
        m_operand.clear();
        m_state = State::EnteringSecond;
    }

    // 同一个操作数里只允许出现一个小数点
    if (m_operand.contains('.'))
        return;

    if (m_operand.isEmpty())
        m_operand = "0";

    m_operand += ".";
    setDisplay(m_operand);
}

void Calculator::applyOperator(const QString &op)
{
    if (m_state == State::Error)
        return;

    if (m_state == State::OperatorPending) {
        // 第二个操作数还没开始输入，直接换掉运算符
        m_pendingOperator = op;
        setExpression(formatNumber(m_firstOperand) + " " + op);
        return;
    }

    if (m_state == State::EnteringSecond) {
        // 连续运算：先把上一段算出来，结果作为新的第一操作数
        double intermediate = 0.0;
        if (!compute(m_firstOperand, m_operand.toDouble(), m_pendingOperator, &intermediate)) {
            showError();
            return;
        }
        m_firstOperand = intermediate;
        setDisplay(formatNumber(m_firstOperand));
    } else {
        m_firstOperand = m_operand.toDouble();
    }

    m_pendingOperator = op;
    m_operand.clear();
    m_state = State::OperatorPending;
    setExpression(formatNumber(m_firstOperand) + " " + op);
}

void Calculator::calculate()
{
    if (m_state == State::Error || m_state == State::ShowingResult)
        return;

    if (m_pendingOperator.isEmpty()) {
        m_operand = formatNumber(m_operand.toDouble());
        setExpression(m_operand + " =");
        setDisplay(m_operand);
        m_state = State::ShowingResult;
        return;
    }

    // 按运算符后直接按等号时，用第一个操作数补齐第二个
    const double secondOperand = m_operand.isEmpty() ? m_firstOperand : m_operand.toDouble();
    const QString expression = formatNumber(m_firstOperand) + " " + m_pendingOperator + " "
                               + formatNumber(secondOperand) + " =";

    double result = 0.0;
    if (!compute(m_firstOperand, secondOperand, m_pendingOperator, &result)) {
        setExpression(expression);
        showError();
        return;
    }

    setExpression(expression);
    m_operand = formatNumber(result);
    m_state = State::ShowingResult;
    setDisplay(m_operand);
}

void Calculator::backspace()
{
    if (m_state != State::EnteringFirst && m_state != State::EnteringSecond)
        return;

    m_operand.chop(1);
    if (m_operand.isEmpty() || m_operand == "-")
        m_operand = "0";

    setDisplay(m_operand);
}

void Calculator::clearAll()
{
    m_state = State::EnteringFirst;
    m_operand = "0";
    m_firstOperand = 0.0;
    m_pendingOperator.clear();

    setExpression(QString());
    setDisplay(m_operand);
}

void Calculator::toggleSign()
{
    if (m_state == State::OperatorPending || m_state == State::Error)
        return;

    if (m_operand.startsWith('-'))
        m_operand.remove(0, 1);
    else if (m_operand != "0")
        m_operand.prepend('-');

    setDisplay(m_operand);
}

void Calculator::setDisplay(const QString &text)
{
    ui->displayEdit->setText(text);
}

void Calculator::setExpression(const QString &text)
{
    ui->expressionLabel->setText(text);
}

void Calculator::showError()
{
    m_state = State::Error;
    m_operand.clear();
    m_pendingOperator.clear();
    setDisplay(QStringLiteral("除数不能为 0"));
}

bool Calculator::compute(double left, double right, const QString &op, double *result)
{
    if (op == "+") {
        *result = left + right;
    } else if (op == "-") {
        *result = left - right;
    } else if (op == "×") {
        *result = left * right;
    } else if (op == "÷") {
        if (right == 0.0)
            return false;
        *result = left / right;
    } else {
        *result = right;
    }
    return true;
}

QString Calculator::formatNumber(double value)
{
    // 避免 0.1+0.2 显示成一长串浮点误差，顺便把 -0 归一成 0
    if (value == 0.0)
        return QStringLiteral("0");
    return QString::number(value, 'g', 15);
}
