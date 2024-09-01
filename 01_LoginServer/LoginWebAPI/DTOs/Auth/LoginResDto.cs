using System.Text.Json.Serialization;

namespace LoginWebAPI.DTOs.Auth
{
    public class LoginResDto
    {        
        [JsonPropertyName("id")]
        public UInt64 Id { get; set; }

        [JsonPropertyName("token")]
        public string Token { get; set; }
    }
}
