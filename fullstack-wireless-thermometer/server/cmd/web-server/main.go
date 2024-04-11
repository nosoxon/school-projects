package main

import (
	"github.com/gin-gonic/gin"
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"

	"net/http"
	"os"
)

const envListenAddress = "LISTEN_ADDRESS"

var (
	ListenAddress string
	Temperature   string
	Displaying    bool = false
)

func getEnv(key string, def string) (val string) {
	val, ok := os.LookupEnv(key)
	if !ok {
		val = def
	}
	return
}

func init() {
	ListenAddress = getEnv(envListenAddress, ":8080")
	if _, ok := os.LookupEnv("DEBUG"); ok {
		log.Logger = log.Output(zerolog.ConsoleWriter{Out: os.Stderr})
	}
}

type sensorForm struct {
	Temperature *string `form:"t"`
}

type displayForm struct {
	Display *string `json:"display"`
}

func postTemperature(c *gin.Context) {
	var form sensorForm
	if err := c.ShouldBind(&form); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"error": "unknown",
		})
		return
	}

	Temperature = *form.Temperature
	if !Displaying {
		c.Status(http.StatusOK)
	} else {
		c.Status(201)
	}
}

func getTemperature(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{
		"temperature": Temperature,
	})
}

func postDisplay(c *gin.Context) {
	var form displayForm
	if err := c.ShouldBind(&form); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"error": "unknown",
		})
		return
	}

	if *form.Display == "on" {
		Displaying = true
	} else if *form.Display == "off" {
		Displaying = false
	}
}

func main() {
	r := gin.Default()
	r.POST("/temperature", postTemperature)
	r.GET("/temperature", getTemperature)
	r.POST("/display", postDisplay)
	r.Run(ListenAddress)
}
