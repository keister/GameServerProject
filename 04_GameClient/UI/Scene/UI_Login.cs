using TMPro;
using UnityEngine.UI;
using UnityEngine;
using UnityEngine.EventSystems;
using System;
using System.Collections;
using System.Text;
using Controller.Network;
using UnityEngine.Networking;


public class UI_Login : UI_Scene
{

    public class CertificateWhore : CertificateHandler
    {
        protected override bool ValidateCertificate(byte[] certificateData)
        {
            return true;
        }
    }
    
    public struct RequestLogin
    {
        public string username;
        public string password;
    }

    public struct ResLogin
    {
        public ulong id;
        public string token;
    }

    
    public enum Buttons
    {
        Btn_Login,
        OkButton,
    }

    public enum InputFields
    {
        Input_ID,
        Input_Password,
    }

    public enum Texts
    {
        Text_ID,
        Text_Password,
        Text_IDLabel,
        Text_PasswordLabel,
        PopUpText,
    }

    public enum Images
    {
        PopUp,
    }
    
    private void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init(); // 📜UI_Button 의 부모인 📜UI_PopUp 의 Init() 호출

        
        Managers.Network.Login.Bind((UInt16)Define.PacketType.S_LOGIN, OnRecvPacket);
        
        Bind<Button>(typeof(Buttons)); // 버튼 오브젝트들 가져와 dictionary인 _objects에 바인딩. 
        Bind<TMP_InputField>(typeof(InputFields)); // 버튼 오브젝트들 가져와 dictionary인 _objects에 바인딩. 
        Bind<TMP_Text>(typeof(Texts)); // 버튼 오브젝트들 가져와 dictionary인 _objects에 바인딩. 
        Bind<Image>(typeof(Images));
        
        GameObject go = GetImage((int)Images.PopUp).gameObject;
        BindEvent(go, (PointerEventData data) => { go.transform.position = data.position; }, Define.UIEvent.Drag);
        GetImage((int)Images.PopUp).gameObject.SetActive(false);
        GetButton((int)Buttons.Btn_Login).gameObject.BindEvent(OnLoginButtonClicked);
        GetButton((int)Buttons.OkButton).gameObject.BindEvent(OnOKButtonClicked);
    }

    
    IEnumerator NetCoroutine()
    {
        RequestLogin req = new RequestLogin() { username = GetInputField((int)InputFields.Input_ID).text, password = GetInputField((int)InputFields.Input_Password).text };

        string json = JsonUtility.ToJson(req);
        
        UnityWebRequest www = UnityWebRequest.Post("https://procademyserver.iptime.org:11731/api/auth/login", 
            json, "application/json");

        www.certificateHandler = new CertificateWhore();
        yield return www.SendWebRequest();
        
        if (www.result != UnityWebRequest.Result.Success)
        {
            Debug.Log($"Fail : {www.error}");
            
            Get<TMP_Text>((int)Texts.PopUpText).text = www.error;
            GetImage((int)Images.PopUp).gameObject.SetActive(true);
        }
        else
        {
            GetInputField((int)InputFields.Input_ID).enabled = true;
            GetInputField((int)InputFields.Input_Password).enabled = true;
            GetButton((int)Buttons.Btn_Login).enabled = true;

            ResLogin res = JsonUtility.FromJson<ResLogin>(www.downloadHandler.text);
            
            
            Debug.Log($"id : {res.id}, toke : {res.token}");



            if (Managers.Network.Login.Connect("127.0.0.1", 11756) == false)
            {
                Get<TMP_Text>((int)Texts.PopUpText).text = "Connect Fail";
                GetImage((int)Images.PopUp).gameObject.SetActive(true);
            }
            Packet pkt = new Packet();
            
            pkt.Write((UInt16)101);
            pkt.Write(res.id);
            pkt.Write(Encoding.UTF8.GetBytes(res.token));
            
            Managers.Network.Login.SendPacket(pkt);
        }
    }
    public void OnLoginButtonClicked(PointerEventData data)
    {
        GetInputField((int)InputFields.Input_ID).enabled = false;
        GetInputField((int)InputFields.Input_Password).enabled = false;
        GetButton((int)Buttons.Btn_Login).enabled = false;
        StartCoroutine(NetCoroutine());
    }

    public void OnOKButtonClicked(PointerEventData data)
    {
        GetImage((int)Images.PopUp).gameObject.SetActive(false);
        GetInputField((int)InputFields.Input_ID).enabled = true;
        GetInputField((int)InputFields.Input_Password).enabled = true;
        GetButton((int)Buttons.Btn_Login).enabled = true;
    }

    public void OnRecvPacket(Packet pkt)
    {
        pkt.Read(out UInt64 accountNo)
            .Read(out Byte status)
            .Read(out byte[] token);

        
        PlayerPrefs.SetInt("AccountId", (int)accountNo);
        PlayerPrefs.SetString("PlayerToken", Encoding.UTF8.GetString(token));
        
        Debug.Log($"{accountNo}, {status}, {Encoding.UTF8.GetString(token)}");
        Debug.Log($"{PlayerPrefs.GetInt("AccountId")}, {PlayerPrefs.GetString("PlayerToken")}, {Encoding.UTF8.GetString(token)}");
        
        if (status == 1)
        {
            Managers.Scene.LoadScene(Define.Scene.ServerSelect);
        }
    }
}