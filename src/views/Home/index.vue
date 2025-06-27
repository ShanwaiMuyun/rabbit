<template>
    <div class="container">
        <HomeCategory />
        <HomeBanner />
    </div>
    <HomeNew />
    <HomeHot />
    <HomeProduct />
    <!-- 测试面板组件 -->
    <!-- <HomePanel title="新鲜好物">
        <div>
            我是新鲜好物的插槽内容
        </div>
    </HomePanel>
    <HomePanel title="人气推荐">
        <div>
            我是人气推荐的插槽内容
        </div>
    </HomePanel> -->
</template>

<script setup>
import HomeBanner from "./components/HomeBanner.vue"
import HomeCategory from "./components/HomeCategory.vue"
import HomeHot from "./components/HomeHot.vue"
import HomeNew from "./components/HomeNew.vue"
import HomeProduct from "./components/HomeProduct.vue"
// import HomePanel from "./components/HomePanel.vue"
</script>