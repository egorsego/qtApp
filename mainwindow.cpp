#include "mainwindow.h"
#include "ui_mainwindow.h"

std::string CASHIER = "ANDROID CASHIER";

MainWindow::MainWindow(QWidget *parent)  : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // logger start
    loggerDaemon = nullptr;
    runLogDaemon();

    ui->setupUi(this);
//  qDebug() << "App path : " << qApp->applicationDirPath();
//  App path :  "/data/app/org.qtproject.example.testAppOldUi--QtfJN_oBzhVPTVu9eZt2g==/lib/arm64"
    logERR("%s:: End", __func__);
}

MainWindow::~MainWindow()
{
    logERR("%s:: Start", __func__);
    delete ui;
}


void MainWindow::on_pbConnect_clicked()
{
    logERR("%s:: Start", __func__);
    qDebug() << " IP = " << ui->leIP->text() << endl;
    libSetIP(ui->leIP->text().toStdString());
    qDebug() << " IP = " << ui->leIP->text() << endl;
    libSetPort(ui->lePort->text().toInt());
    libSetPasswd(Country::RF);
    setDebugLevel(DEBUG_LEVEL::DEBUG_ON);
    qDebug() << "========= commandStart res = " << commandStart() << endl;
}

void MainWindow::on_pbScroll_clicked()
{
    logERR("%s:: Start", __func__);
    qDebug() << "========= scrollPaper = " << pirit_io.scrollPaper() << endl;
}

void MainWindow::on_pbPrintSDoc_clicked()
{
    logWARN("%s:: Start", __func__);
    libOpenDocument(0x01, 22, (char*)CASHIER.c_str(), 1);
    for(int i = 0; i < 4; i++ )
    {
        libPrintString("Hello from Android!", FONTS::FONT_BIG);
    }
    CloseDocParam param1;
    libCloseDocument(param1);
}


void MainWindow::on_pbXReport_clicked()
{
    logWARN("%s:: PRINT X REPORT = %d", __func__, libPrintXReport( CASHIER.c_str()));
//    qDebug() << "========= PRINT X REPORT = " << libPrintXReport( CASHIER.c_str()) << endl;
}

void MainWindow::on_pbOpenShift_clicked()
{
    logWARN("%s:: OPEN SHIFT = %d", __func__, libOpenShift(CASHIER.c_str()) );
//    qDebug() << "========= OPEN SHIFT = " << libOpenShift( CASHIER.c_str() ) << endl;
}

void MainWindow::on_pbReciept_clicked()
{
    logWARN("%s:: Start", __func__);
    // 1 Открыть чек
    int res = libOpenDocument( DOC_TYPES::DOC_TYPE_SALE,
                               DOC_FORMATS::DOC_FORMAT_PACKET,
                               DOC_REQ_PRINT::DOC_REQ_PRINT_NORMAL,
                               1,
                               CASHIER.c_str(),
                               11,
                               SNO_TYPES::SNO_OSN );
    if(res != 0)
    {return;}

    // 2 Добавить позиции
    PiritLib::Position position;
    for(int i = 0; i < 10; i++)
    {
        position.goodsName = "Android Product "+ std::to_string(i);                    // (Строка[0...224]) Название товара
        position.barcode= "12345"+ std::to_string(i);                      // (Строка[0..18]) Артикул или штриховой код товара/номер ТРК
        position.quantity = 1;                    // (Дробное число) Количество товара в товарной позиции
        position.price = 100;                        // (Дробное число) Цена товара по данному артикулу
        position.taxNumber  = SNO_OSN;           // (Целое число) Номер ставки налога
        position.numGoodsPos = i;                      // (Строка[0..4]) Номер товарной позиции
        position.numDepart  =1;           // (Целое число 1..16) Номер секции
        position.signMethodCalculation  = 4;     // Целое число) Признак способа расчета  (1 - Предоплата 100%)
        position.signCalculationObject  = 1;     // Целое число) Признак предмета расчета (1 - ????)
        res = libAddPosition(position);
        qDebug() << "========= RECIEPT:: ADD POSITION = " << res << endl;
    }
    if(res != 0)
    {return;}

    // 3 Добавить оплату  в открытый документ
    res = libAddPayment( 1, 1000,   "" );

    qDebug() << "========= RECIEPT:: ADD PAYMENT = " << res << endl;
    if(res != 0)
    {return;}

//    CloseDocParam closeDocParam;
//    closeDocParam.cutPaper = 0x00;
//    closeDocParam.buyerAddress = "buyer Address";
//    closeDocParam.buyerName = "buyer Name";
//    closeDocParam.buyerInn = "12345678901";
//    CloseDocResult closeRes(libCloseDocument(closeDocParam));


    CloseDocResult closeRes(libCloseDocumentLite(1, "buyerAddress@dreamkas.ru", 0));

    qDebug() << "========= RECIEPT:: CLOSE DOCUMENT = " << closeRes.errCode << endl;
}

void MainWindow::on_pushButton_clicked()
{
    logWARN("%s:: CLOSE SHIFT = %d", __func__, libPrintZReport(CASHIER.c_str(), 0) );
//    qDebug() << "========= CLOSE SHIFT = " << libPrintZReport(CASHIER.c_str(), 0) << endl;
}


void MainWindow::runLogDaemon()
{
//    qDebug()  << __func__ <<": requestAndroidPermissions result = " << (requestAndroidPermissions() ? "true" : "false")  << endl;
    logWARN("%s:: requestAndroidPermissions = %s", __func__, (requestAndroidPermissions() ? "true" : "false") );

    if( loggerDaemon == nullptr)
    {
        loggerDaemon = new LoggerDaemon;
        QThread* logThread = new QThread;
        loggerDaemon->moveToThread(logThread);

        connect(logThread, SIGNAL(started()), loggerDaemon, SLOT(start()));
//        connect(loggerDaemon, SIGNAL(started()), this, SLOT(on_cbRunLogger_stateChanged()));
        logThread->start();
    }
    else
    {
        qDebug()  << __func__ <<": log daemon already started! Skip start "  << endl;
        logWARN("%s:: log daemon already started! Skip start", __func__);
    }
}


bool MainWindow::requestAndroidPermissions()
{
#ifdef Q_OS_ANDROID   // Q_OS_WIN - windows
    //Request requiered permissions at runtime
    const QVector<QString> permissions({"android.permission.WRITE_EXTERNAL_STORAGE",
                                        "android.permission.READ_EXTERNAL_STORAGE"});
/*  others values:
const QVector<QString> permissions({"android.permission.ACCESS_COARSE_LOCATION",
                                    "android.permission.BLUETOOTH",
                                    "android.permission.BLUETOOTH_ADMIN",
                                    "android.permission.INTERNET",
                                    "android.permission.WRITE_EXTERNAL_STORAGE",
                                    "android.permission.READ_EXTERNAL_STORAGE"});
*/
    for(const QString &permission : permissions)
    {
        auto result = QtAndroid::checkPermission(permission);
        if(result == QtAndroid::PermissionResult::Denied)
        {
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if(resultHash[permission] == QtAndroid::PermissionResult::Denied)
                return false;
        }
    }
#else
    qDebug() << "-----======== Q_OS_ANDROID NOT DEFINED! ========-------" << endl;
#endif
    return true;
}

void MainWindow::OutText(const QString& str, OUTMODE mode)
{
    switch(mode)
    {
        case OUTMODE::NORMAL_T:
        {
            ui->teOutput->append(str + "\r\n");
            break;
        }
        case OUTMODE::GREEN_T:
        {
            ui->teOutput->append("<span style=\"color:#008800;\" >" + str + "</span>\r\n");
            break;
        }
    }
}


void MainWindow::on_pbStartExchange_clicked()
{
   OutText("Кнопка Обмена с хабом нажата!");
   logERR("Кнопка Обмена с хабом нажата!");
   if( sendHandshakeToHub() )
   {
       sendPollToHub();
   }
}

bool MainWindow::sendHandshakeToHub()
{
    logERR("%s:: start!", __func__);
    OutText("HANDSHAKE...");
    int task_manager_ERR{0};
    setLogDBcode_CP866();
    std::string answ, req;
    task_manager_ERR = he.sendHandshake( &config_data, req, answ);
    setLogDBcode_UTF8();
    OutText("HANDSHAKE в хаб:");
    OutText(req.c_str(), OUTMODE::GREEN_T);
    OutText("Ответ от хаба на HANDSHAKE:");
    OutText(answ.c_str(), OUTMODE::GREEN_T);
    if(task_manager_ERR == CAB_OK)
    {
        // HANDSHAKE завершён
        OutText("HANDSHAKE Прошел успешно!");
        logERR( "%s:: HANDSHAKE SUCCESS", __func__);
        return true;
    }
    else
    {
        // HANDSHAKE не завершён
        OutText("Ошибка при попытке HANDSHAKE!");
        logERR( "%s:: HANDSHAKE FAILED!", __func__);
        return false;
    }

}

void MainWindow::sendPollToHub()
{
   logERR("%s:: start!", __func__);
   OutText("POLL...");
   int task_manager_ERR{0};
   setLogDBcode_CP866();
   std::string answ, req;
   task_manager_ERR = he.sendPollNew(&config_data, &json_poll, req, answ);
   setLogDBcode_UTF8();
   OutText("POLL в хаб:");
   OutText(req.c_str(), OUTMODE::GREEN_T);
   OutText("Ответ от хаба на POLL:");
   OutText(answ.c_str(), OUTMODE::GREEN_T);

   if (task_manager_ERR == CAB_OK)
   {
       logERR("POLLING SUCCESS");
       OutText("POLL Прошел успешно!");
       if ( !json_poll.response.tasks.empty() )
       {
           logERR("%s:: !json_poll.response.tasks.empty()");
           OutText("Пришли таски!");
       }
   }
   else if (task_manager_ERR == CAB_NEED_HANDSHAKE)
   {
       logERR("%s:: task_manager_ERR == CAB_NEED_HANDSHAKE");
       OutText("Требуется HANDSHAKE!");
       JSON::json_poll_clear(&json_poll, JSON::POLL_RESPONSE);
//                               need_handshake_set();
   }
   else if (task_manager_ERR == CAB_NEED_REGISTERED)
   {
       if (config_data.isCabinetEnable)
       {
           logERR("%s::  task_manager_ERR == CAB_NEED_REGISTERED");
           OutText("Требуется CAB_NEED_REGISTERED!");

           /*
           // Касса успешно отключена
           config_data.isCabinetEnable = false;
           config_data.cabinetRegEmail.clear();
           sql->updateOptimized( SAVE_CFG, IS_CABINET_ENABLE | CABINET_REG_EMAIL, &config_data );
           // Вырубать авто открытие/закрытие смены при отключении от кабинета
           isAutoOpenShiftEnable = false;
           config_data.isAutoOpenShiftEnable = isAutoOpenShiftEnable;
           sql->updateOptimized( SAVE_CFG, AUTO_OPEN_SHIFT, &config_data );
           lcd->lcd_draw_mess("Касса", "успешно", "отключена от", "Кабинета Дримкас", 2);
       */}
   }
   else
   {
       OutText("TERMINAL_TASK = FREE");
       logERR("%s:: TERMINAL_TASK = FREE", __func__);
        OutText("TERMINAL_TASK = FREE");
   }
}
