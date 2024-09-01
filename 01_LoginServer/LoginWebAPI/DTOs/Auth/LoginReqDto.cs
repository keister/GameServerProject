using System.Text.Json.Serialization;

namespace LoginWebAPI.DTOs.Auth
{
    public class LoginReqDto
    {
        [JsonPropertyName("username")]
        public string UserName { get; set; }

        [JsonPropertyName("password")]
        public string Password { get; set; }
    }
}
