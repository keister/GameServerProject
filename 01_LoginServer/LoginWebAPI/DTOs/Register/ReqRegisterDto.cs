using System.Text.Json.Serialization;

namespace LoginWebAPI.DTOs.Register
{
    public class ReqRegisterDto
    {
        [JsonPropertyName("username")]
        public string UserName { get; set; }
        [JsonPropertyName("password")]
        public string Password { get; set; }
    }
}
