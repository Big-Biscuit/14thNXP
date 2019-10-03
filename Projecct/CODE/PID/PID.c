#include "PID.h"

PID_S S_D5_PID, MOTOR_PID;	//�������͵����PID�����ṹ��
float S_D5[7][4] = {{2.0, 200, 7.8, 0},{2.0, 230, 8.0, 0}, {2.0, 250, 10.0, 0}, {2.0, 260, 10.0, 0}, {2.0, 270, 15.0, 0}, {3.3, 275, 8.0, 0}, {3.3, 300, 8.0, 0}};	//���PID {2.5, 0, 5.5, 250}

int S_D5_MOTOR[7] = {10, 10, 15, 15, 15, 15, 15};//���ֲ���
float DKP = 0;	

//λ��ʽPID������ʼ��
float MOTOR[3] = {4, 0.7, 0};	//���PID 1.6, 0, 1.2  /4, 0.7, 0

void PlacePID_Init(PID_S *sptr)
{
	sptr->SumError = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]
}
//����ʽPID������ʼ��
void IncPID_Init(PID_S *sptr)
{
	sptr->SumError = 0;
	sptr->LastError = 0;	//Error[-1]
	sptr->PrevError = 0;	//Error[-2]	
	sptr->LastSpeed = 0;
}

//λ��ʽPID�������
int32 PlacePID_Control(PID_S *sprt, int32 NowPiont, int32 SetPoint)
{
	int32 iError;	//��ǰ���
	float Actual;	//���ó���ʵ�����ֵ
//	float	Kp;		//��̬P

	iError = SetPoint - NowPiont;	//���㵱ǰ���
	
//	Kp = 1.0 * (iError*iError) / S_D5[Mode_Set][KT] + S_D5[Mode_Set][KP];	//Pֵ���ֵ�ɶ��κ�����ϵ
//	DKP = Kp;
//	Actual = Kp * iError + S_D5[Mode_Set][KD] * (iError - sprt->LastError);//ֻ��PD
	Actual = S_D5[Mode_Set][KP] * iError + S_D5[Mode_Set][KD] * (iError - sprt->LastError);
	
	sprt->LastError = iError;		//�����ϴ����

	Actual = range_protect(Actual, -(S_D5_L - S_D5_MID), S_D5_L - S_D5_MID);
	return S_D5_MID - Actual;
}

//λ��ʽPID�������Obstacle
int32 PlacePID_Control_Obstacle(PID_S *sprt, int32 NowPiont, int32 SetPoint)
{
	int32 iError;	//��ǰ���
	float Actual;	//���ó���ʵ�����ֵ

	iError = -(SetPoint - NowPiont);	//���㵱ǰ���
	
	Actual = 10 * iError + 2 * (iError - sprt->LastError);
	
	sprt->LastError = iError;		//�����ϴ����

	Actual = range_protect(Actual, -(S_D5_L - S_D5_MID), S_D5_L - S_D5_MID);
	return S_D5_MID - Actual;
}

//λ��ʽPID�������Broken
int32 PlacePID_Control_Broken(PID_S *sprt, int32 NowPiont, int32 SetPoint)
{
	int32 iError;	//��ǰ���
	float Actual;	//���ó���ʵ�����ֵ

	iError = SetPoint - NowPiont;	//���㵱ǰ���
	
	Actual = 0.2 * iError + 1 * (iError - sprt->LastError);//p0.2 d1.0
	
	sprt->LastError = iError;		//�����ϴ����

	Actual = range_protect(Actual, -(S_D5_L - S_D5_MID), S_D5_L - S_D5_MID);
	return S_D5_MID - Actual;
}

int32 PID_Cascade(PID_S *sptr, int32 ActualSpeed, int32 SetSpeed)
{
	//��ǰ������Ϊ�Ĵ���������ֻ���������ͺ��ַ��ͱ�������������ٶ�
	register int32 iError,	    //��ǰ���
				   Increase;	//���ó���ʵ������
	
	iError = SetSpeed - ActualSpeed;//���㵱ǰ���
	
	Increase = - MOTOR[KP] * (ActualSpeed - sptr->LastSpeed)	//���ٶ�
			   + MOTOR[KI] * iError
			   + MOTOR[KD] * (iError - 2 * sptr->LastError + sptr->PrevError);
//	Increase = MOTOR[KP] * iError + MOTOR[KD] * (iError - sptr->LastError);
	
	sptr->PrevError = sptr->LastError;	//����ǰ�����
	sptr->LastError = iError;		  	//�����ϴ����
	sptr->LastSpeed = ActualSpeed;		//�����ϴ��ٶ�
	
	return Increase;   
}	
