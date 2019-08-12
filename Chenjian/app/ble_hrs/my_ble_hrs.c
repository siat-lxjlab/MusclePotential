#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_MY_HRS)
#include "my_ble_hrs.h"
#include <string.h>
#include "ble_srv_common.h"

//�����볤�ȣ�1���ֽ�
#define OPCODE_LENGTH 1                                                              
//������ȣ�2���ֽ�
#define HANDLE_LENGTH 2                                                             
//BLEһ���ܴ�������ʲ���������ֽ���������BLE�ں�Э�飬MTU������䵥Ԫ��������1�ֽ�Opcode + 2�ֽ�����handle + ��Ч�غ�
#define MAX_HRM_LEN      (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH) 
//���ʲ�����ʼֵ
#define INITIAL_VALUE_HRM                       0                                    /**< Initial Heart Rate Measurement value. */

/*----���ʲ���flagλ�ζ��� ����SIG�����ġ�HRS_SPEC_VV10r00������--*/
//����ֵ���ݸ�ʽλ =0:UINT8;=1:UINT16
#define HRM_FLAG_MASK_HR_VALUE_16BIT            (0x01 << 0) 
//�������Ӵ����λ�����֧�ֽӴ���⣬���豸��⵽û�нӴ���Ƥ�����߽Ӵ�������λ1Ӧ����Ϊ0����������Ϊ1
#define HRM_FLAG_MASK_SENSOR_CONTACT_DETECTED   (0x01 << 1)  
//�������Ӵ�֧��λ =0:��֧�ִ������Ӵ���⣬=1��֧��
#define HRM_FLAG_MASK_SENSOR_CONTACT_SUPPORTED  (0x01 << 2) 
//��������״̬λ,ָʾ���������ֶ��Ƿ���������ʲ��������С�������ڣ���λӦ����Ϊ1����������Ϊ0
#define HRM_FLAG_MASK_EXPENDED_ENERGY_INCLUDED  (0x01 << 3) 
//RR-Intervalλ��ָʾRR-Intervalֵ�Ƿ���������ʲ��������С����һ�����߶��RR-Intervalֵ���ڣ���λӦ����Ϊ1����������Ϊ0��                          
#define HRM_FLAG_MASK_RR_INTERVAL_INCLUDED      (0x01 << 4)                          


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
//�����¼���ʾ���ӽ�������ʱӦ�����Ӿ���������ʷ���ʵ��p_hrs
static void on_connect(ble_hrs_t * p_hrs, ble_evt_t const * p_ble_evt)
{
    p_hrs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
//�Ͽ������¼���ʾ�����Ѿ��Ͽ�����ʱӦ�������ʷ���ʵ���е����Ӿ��Ϊ��Ч��BLE_CONN_HANDLE_INVALID��
static void on_disconnect(ble_hrs_t * p_hrs, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_hrs->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling write events to the Heart Rate Measurement characteristic.
 *
 * @param[in]   p_hrs         Heart Rate Service structure.
 * @param[in]   p_evt_write   Write event received from the BLE stack.
 */
static void on_hrm_cccd_write(ble_hrs_t * p_hrs, ble_gatts_evt_write_t const * p_evt_write)
{
    if (p_evt_write->len == 2)
    {
        //дCCCD������֪ͨ��״̬
        if (p_hrs->evt_handler != NULL)
        {
            ble_hrs_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_HRS_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_HRS_EVT_NOTIFICATION_DISABLED;
            }

            p_hrs->evt_handler(p_hrs, &evt);
        }
    }
}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_hrs       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_hrs_t * p_hrs, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (p_evt_write->handle == p_hrs->hrm_handles.cccd_handle)
    {
        on_hrm_cccd_write(p_hrs, p_evt_write);
    }
}

//���ʷ���BLE�¼������ߵ��¼��ص�����
void ble_hrs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    //����һ�����ʽṹ��ָ�벢ָ�����ʽṹ��
	  ble_hrs_t * p_hrs = (ble_hrs_t *) p_context;
    //�ж��¼�����
    switch (p_ble_evt->header.evt_id)
    {       
			  case BLE_GAP_EVT_CONNECTED://�����¼�
			      //�����Ӿ�����浽���ʷ���ʵ��p_hrs�ġ�conn_handle��
            on_connect(p_hrs, p_ble_evt);
            break;
        case BLE_GAP_EVT_DISCONNECTED://���ӶϿ��¼�
			      // �������ʷ���ʵ��p_hrs�е����Ӿ����conn_handle��ΪBLE_CONN_HANDLE_INVALID
            on_disconnect(p_hrs, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE://д�¼�
		        //ʹ�ܻ�ر����ʲ���������֪ͨ
            on_write(p_hrs, p_ble_evt);
            break;
        default:
            break;
    }
}

//��ʼ�����ʷ���
uint32_t ble_hrs_init(ble_hrs_t * p_hrs, const ble_hrs_init_t * p_hrs_init)
{
    uint32_t              err_code;
	  //����UUID�ṹ�����
    ble_uuid_t            ble_uuid;
	  //�������������ṹ�����
    ble_add_char_params_t add_char_params;
    uint8_t               initial_hrm[9];

    //��ʼ�����ʷ���ṹ��
	  //�������ʷ����ʼ���ṹ���е����ʷ����¼����
    p_hrs->evt_handler                 = p_hrs_init->evt_handler;
	  //�������ʷ����ʼ���ṹ���е����ʷ����¼����
    p_hrs->is_sensor_contact_supported = p_hrs_init->is_sensor_contact_supported;
	  //���Ӿ����ʼֵ����Ϊ����Ч�����Ӿ��
    p_hrs->conn_handle                 = BLE_CONN_HANDLE_INVALID;
	  //�������Ӵ�����Ϊfalse����δ�Ӵ�
    p_hrs->is_sensor_contact_detected  = false;
	  //rr_interval������ʼֵ����Ϊ0
    p_hrs->rr_interval_count           = 0;
	  //���õ�ǰһ�οɷ��͵�������ʲ������ݳ���
    p_hrs->max_hrm_len                 = MAX_HRM_LEN;

    //���ʷ����UUID
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_HEART_RATE_SERVICE);
    //��ӷ������Ա�
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_hrs->service_handle);
    //��鷵�صĴ������
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    //�������ʲ�������
		//���ò���֮ǰ������add_char_params
    memset(&add_char_params, 0, sizeof(add_char_params));
    //���ʲ���������UUID
    add_char_params.uuid              = BLE_UUID_HEART_RATE_MEASUREMENT_CHAR;
		//�������ʲ�������ֵ����󳤶�
    add_char_params.max_len           = MAX_HRM_LEN;
		//�������ʲ�������ֵ�ĳ�ʼ����
    add_char_params.init_len          = 0;
    add_char_params.p_init_value      = initial_hrm;
		//�������ʲ���������ֵ����Ϊ�ɱ䳤��
    add_char_params.is_var_len        = true;
		//���ô�����λ�����������ʣ�֧��֪ͨ
    add_char_params.char_props.notify = 1;
    add_char_params.cccd_write_access = p_hrs_init->hrm_cccd_wr_sec;
    //Ϊ���ʷ���������ʲ�������
    err_code = characteristic_add(p_hrs->service_handle, &add_char_params, &(p_hrs->hrm_handles));
		//��鷵�صĴ������
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    //������ʷ������������λ��������Ϊ���ʷ�����Ӵ�����λ������
    if (p_hrs_init->p_body_sensor_location != NULL)
    {
        //��ʼ������֮ǰ��������add_char_params
        memset(&add_char_params, 0, sizeof(add_char_params));
        //���ô�����λ��������UUID
        add_char_params.uuid            = BLE_UUID_BODY_SENSOR_LOCATION_CHAR;
			  //���ô�����λ������ֵ����󳤶�
        add_char_params.max_len         = sizeof(uint8_t);
			  //���ô�����λ������ֵ�ĳ�ʼ����
        add_char_params.init_len        = sizeof(uint8_t);
			  //���ô�����λ�������ĳ�ʼֵ
        add_char_params.p_init_value    = p_hrs_init->p_body_sensor_location;
			  //���ô�����λ�����������ʣ�֧�ֶ�ȡ
        add_char_params.char_props.read = 1;
			  //���ö�������λ������ֵ�İ�ȫ����
        add_char_params.read_access     = p_hrs_init->bsl_rd_sec;
        //Ϊ���ʷ�����Ӵ�����λ������
        err_code = characteristic_add(p_hrs->service_handle, &add_char_params, &(p_hrs->bsl_handles));
			  //��鷵�صĴ������
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }

    return NRF_SUCCESS;
}

//�������ʲ�������֪ͨ������������
uint32_t ble_hrs_heart_rate_measurement_send(ble_hrs_t * p_hrs, uint16_t heart_rate)
{
//  static uint8_t rr_val = 0;
	  uint32_t err_code;
	  //flag��ʼֵ����Ϊ0x15�����������ʵ����ݸ�ʽΪUINT16��֧�ִ������Ӵ���⡢��֧��������⡢���������а���RR-Interval
	  uint8_t flags = 0x15;

    //���������Ч���������ʲ�������֪ͨ
    if (p_hrs->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint8_t                hrm_data[3];
        uint16_t               hvx_len;
        ble_gatts_hvx_params_t hvx_params;
        //д�����ʲ�����������ֵ       
		    if (p_hrs->is_sensor_contact_detected)
        {
            //���´������Ӵ����״̬
			      flags |= HRM_FLAG_MASK_SENSOR_CONTACT_DETECTED;
        }
				hrm_data[0] = flags;
			  hrm_data[1] = heart_rate;
			  hrm_data[2] = heart_rate>>8;
				
	//		  hrm_data[3] = rr_val+10;
	//		  hrm_data[4] = 0x01;
	//		  hrm_data[5] = rr_val+5;
	//		  hrm_data[6] = 0x01;
	//		  hrm_data[7] = rr_val;
	//		  hrm_data[8] = 0x01;
			  //Ϊ�˷���۲����ݣ�ÿ�η��ͺ����ݼ�1
		//	  rr_val++;
			  //����ֵ����Ϊ10
				
			  hvx_len = 3;
			
        //����֮ǰ������
        memset(&hvx_params, 0, sizeof(hvx_params));
        //���ʲ�������ֵ���
        hvx_params.handle = p_hrs->hrm_handles.value_handle;
				//����Ϊ֪ͨ
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
			  //���͵����ݳ���
        hvx_params.p_len  = &hvx_len;
			  //���͵�����
        hvx_params.p_data = hrm_data;
        //�������ʲ�������֪ͨ
        err_code = sd_ble_gatts_hvx(p_hrs->conn_handle, &hvx_params);

    }
    else//��������״̬��Ч�Ĵ������
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }
    return err_code;
}
//���´������Ӵ����λ
void ble_hrs_sensor_contact_detected_update(ble_hrs_t * p_hrs, bool is_sensor_contact_detected)
{
    p_hrs->is_sensor_contact_detected = is_sensor_contact_detected;
}

#endif // NRF_MODULE_ENABLED(BLE_HRS)
