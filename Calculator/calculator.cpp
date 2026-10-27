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

    setDisplay("0");
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::appendDigit(const QString &digit)
{
    if (m_state == State::ShowingResult) {
        m_operand.clear();
        m_state = State::EnteringFirst;
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
    if (m_state == State::ShowingResult) {
        m_operand.clear();
        m_state = State::EnteringFirst;
    }
    if (m_state == State::OperatorPending) {
        m_operand.clear();
        m_state = State::EnteringSecond;
    }

    if (m_operand.isEmpty())
        m_operand = "0";

    m_operand += ".";
    setDisplay(m_operand);
}

void Calculator::applyOperator(const QString &op)
{
    m_firstOperand = m_operand.toDouble();
    m_pendingOperator = op;
    m_operand.clear();
    m_state = State::OperatorPending;
}

void Calculator::calculate()
{
    const double secondOperand = m_operand.toDouble();
    double result = secondOperand;

    if (m_pendingOperator == "+")
        result = m_firstOperand + secondOperand;
    else if (m_pendingOperator == "-")
        result = m_firstOperand - secondOperand;
    else if (m_pendingOperator == "×")
        result = m_firstOperand * secondOperand;
    else if (m_pendingOperator == "÷")
        result = m_firstOperand / secondOperand;

    m_operand = QString::number(result, 'g', 15);
    m_state = State::ShowingResult;
    setDisplay(m_operand);
}

void Calculator::setDisplay(const QString &text)
{
    ui->displayEdit->setText(text);
}
