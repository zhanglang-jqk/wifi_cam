#include <WiFi.h>

const char *_ssid = "host";
const char *_password = "wocaonima88jqk";

// 116.5.192.73
const IPAddress serverIP(116, 5, 192, 73); //�����ʵĵ�ַ
uint16_t videoServerPort = 8080;               //�������˿ں�

WiFiClient client; //����һ���ͻ��˶����������������������

void setup()
{
    Serial.begin(115200);
    Serial.println();

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); //�ر�STAģʽ��wifi���ߣ������Ӧ�ٶ�
    WiFi.begin(_ssid, _password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());
}

void loop()
{
    Serial.println("���Է��ʷ�����");
    if (client.connect(serverIP, videoServerPort)) //���Է���Ŀ���ַ
    {
        Serial.println("���ʳɹ�");

        client.print("Hello world!");                    //���������������
        while (client.connected() || client.available()) //��������ӻ����յ���δ��ȡ������
        {
            if (client.available()) //��������ݿɶ�ȡ
            {
                String line = client.readStringUntil('\n'); //��ȡ���ݵ����з�
                Serial.print("��ȡ�����ݣ�");
                Serial.println(line);
                client.write(line.c_str()); //���յ������ݻط�
            }
        }
        Serial.println("�رյ�ǰ����");
        client.stop(); //�رտͻ���
    }
    else
    {
        Serial.println("����ʧ��");
        client.stop(); //�رտͻ���
    }
    delay(5000);
}
