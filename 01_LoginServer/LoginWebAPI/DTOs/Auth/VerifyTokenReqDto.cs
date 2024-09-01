using System.Text.Json.Serialization;

namespace LoginWebAPI.DTOs.Auth
{
    public class VerifyTokenReqDto
    {
        [JsonPropertyName("token")]
        public string Token { get; set; }
    }
}
