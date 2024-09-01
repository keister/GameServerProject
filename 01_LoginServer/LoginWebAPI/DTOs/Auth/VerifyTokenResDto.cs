using System.Text.Json.Serialization;

namespace LoginWebAPI.DTOs.Auth
{
    public class VerifyTokenResDto
    {
        [JsonPropertyName("id")]
        public UInt64 Id { get; set; }

        [JsonPropertyName("username")]
        public string UserName { get; set; }
    }
}
