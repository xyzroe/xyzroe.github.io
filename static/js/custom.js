document.addEventListener("DOMContentLoaded", function () {
  var flagIcon = document.querySelector(".flag-icon");
  var languageList = document.querySelector(".language-list");

  flagIcon.addEventListener("click", function () {
    languageList.style.display = languageList.style.display === "none" ? "flex" : "none";
  });

  document.addEventListener("click", function (event) {
    if (!flagIcon.contains(event.target) && !languageList.contains(event.target)) {
      languageList.style.display = "none";
    }
  });

  // Handle images with width/height attributes
  var contentImages = document.querySelectorAll(".content img");
  contentImages.forEach(function (img) {
    if (img.hasAttribute("width")) {
      var widthAttr = img.getAttribute("width");
      var containerWidth = img.parentElement.offsetWidth;

      // Check if width is a percentage
      if (widthAttr.includes("%")) {
        img.style.width = widthAttr;
      } else {
        var originalWidth = parseInt(widthAttr);
        if (originalWidth > containerWidth) {
          img.style.width = "100%";
        } else {
          img.style.width = originalWidth + "px";
        }
      }
    }

    if (img.hasAttribute("height")) {
      var heightAttr = img.getAttribute("height");
      var widthAttr = img.getAttribute("width");

      // Check if height is a percentage
      if (heightAttr.includes("%")) {
        img.style.height = heightAttr;
      } else {
        var originalHeight = parseInt(heightAttr);
        var originalWidth = widthAttr ? parseInt(widthAttr) : img.naturalWidth;

        // If width was scaled down, scale height proportionally
        if (img.style.width && !widthAttr.includes("%") && img.style.width !== originalWidth + "px") {
          var scaleFactor = img.offsetWidth / originalWidth;
          img.style.height = originalHeight * scaleFactor + "px";
        } else if (!widthAttr.includes("%")) {
          img.style.height = originalHeight + "px";
        }
      }
    }
  });

  // Handle code accordion toggle
  var tabLabels = document.querySelectorAll(".tab-label");
  tabLabels.forEach(function (label) {
    label.addEventListener("click", function (e) {
      // Prevent click on download link from toggling
      if (e.target.tagName === "A" || e.target.closest("a")) {
        return;
      }

      var tabId = this.getAttribute("data-tab-id");
      var content = document.getElementById("tab-" + tabId);
      var toggle = this.querySelector(".tab-toggle");

      this.classList.toggle("active");
      content.classList.toggle("active");

      // Change +/- icon
      if (this.classList.contains("active")) {
        toggle.textContent = "−";
      } else {
        toggle.textContent = "+";
      }
    });
  });
});
